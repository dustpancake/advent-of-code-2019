// include source for access to static functions
#include <gtest/gtest.h>
#include <string>

// Macros from `opcomp.c` that are otherwise scoped
#define P_MODE_FIRST(param_modes) param_modes & 15
#define P_MODE_SECOND(param_modes) (param_modes >> 4) & 15
#define P_MODE_THIRD(param_modes) (param_modes >> 8) & 15

// #define DEBUG_ON

extern "C" {
#include "opcomp.c"
};

class OpCompTest : public ::testing::Test {

protected:
  void SetUp() override {
    // ran before each test in the suite
    oc = opcomp_new();
  }

  void TearDown() override {
    // clear pointers after each test in suite
    opcomp_free(&oc);
  }

  void load_prog(std::initializer_list<opc_t> prog) {
    opc_t *test_prog;
    // free currently loaded program
    if (oc.prog != nullptr) {
      free(oc.prog);
    }

    // alloc
    test_prog = (opc_t *)malloc(prog.size() * sizeof(opc_t));
    // copy prog
    opc_t i = 0;
    for (opc_t a : prog) {
      test_prog[i++] = a;
    }
    oc.prog = test_prog;
    oc.plen = prog.size();
    oc.ip = 0;
  }

  testing::AssertionResult prog_is(std::initializer_list<opc_t> prog) {
    int i = 0;

    if ((int) prog.size() != oc.plen) {
      return testing::AssertionFailure()
             << "Expected program size " << prog.size() << ", is " << oc.plen;
    }

    for (opc_t a : prog) {
      // sanity check

      if (oc.prog[i] != a) {
        return testing::AssertionFailure()
               << "Opcode at position " << i << " should be " << a << ", is "
               << oc.prog[i];
      }
      ++i;
    }

    return testing::AssertionSuccess();
  }

  testing::AssertionResult output_is(std::initializer_list<opc_t> output) {
    int i = 0;

    if ((int) output.size() != oc.outlen) {
      return testing::AssertionFailure()
             << "Expected output size " << output.size() << ", is " << oc.outlen;
    }

    for (opc_t a : output) {
      // sanity check

      if (oc.output[i] != a) {
        return testing::AssertionFailure()
               << "Output at position " << i << " should be " << a << ", is "
               << oc.output[i];
      }
      ++i;
    }

    return testing::AssertionSuccess();
  }

  OpComp oc;
};

TEST_F(OpCompTest, AddPosition) {
  load_prog({0, 1, 2, 0});

  int retval = opcomp_add(&oc, 0);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.prog[0], 3);
  ASSERT_EQ(oc.ip, 4);
}

TEST_F(OpCompTest, MultPosition) {
  load_prog({0, 1, 2, 0});

  int retval = opcomp_mult(&oc, 0);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.prog[0], 2);
  ASSERT_EQ(oc.ip, 4);
}

TEST_F(OpCompTest, AddImmediate) {
  load_prog({0, 13, 21, 0});

  int retval = opcomp_add(&oc, 0x11);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.prog[0], 34);
  ASSERT_EQ(oc.ip, 4);
}

TEST_F(OpCompTest, MultImmediate) {
  load_prog({0, 5, 3, 0});

  int retval = opcomp_mult(&oc, 0x11);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.prog[0], 15);
  ASSERT_EQ(oc.ip, 4);
}

TEST_F(OpCompTest, AddMixed) {
  load_prog({0, 3, 18, 2});

  int retval = opcomp_add(&oc, 0x10);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.prog[2], 20);
  ASSERT_EQ(oc.ip, 4);
}

TEST_F(OpCompTest, MultMixed) {
  load_prog({2, 4, 0, 3});

  int retval = opcomp_mult(&oc, 0x01);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.prog[3], 8);
  ASSERT_EQ(oc.ip, 4);
}

TEST_F(OpCompTest, Input) {
  load_prog({3, 0});

  opc_t input[1] = {13};
  opcomp_copy_to_input(&oc, input, 1);

  int retval = opcomp_inp(&oc, 0);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.prog[0], 13);
  ASSERT_EQ(oc.ip, 2);
}

TEST_F(OpCompTest, OutputPosition) {
  load_prog({4, 0});

  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_out(&oc, 0);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.output[0], 4);
  ASSERT_EQ(oc.ip, 2);
}

TEST_F(OpCompTest, OutputImmediate) {
  load_prog({4, 0});

  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_out(&oc, 1);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.output[0], 0);
  ASSERT_EQ(oc.ip, 2);
}

TEST_F(OpCompTest, RBOImmediate) {
  load_prog({9, 15});

  int retval = opcomp_rbo(&oc, 1);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.rbo, 15);
  ASSERT_EQ(oc.ip, 2);
}

TEST_F(OpCompTest, RBOPosition) {
  load_prog({9, 0});

  int retval = opcomp_rbo(&oc, 0);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.rbo, 9);
  ASSERT_EQ(oc.ip, 2);
}

TEST_F(OpCompTest, Day2TProg1) {
  load_prog({1, 0, 0, 0, 99});

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  EXPECT_TRUE(prog_is({2, 0, 0, 0, 99}));
}

TEST_F(OpCompTest, Day2TProg2) {
  load_prog({2, 3, 0, 3, 99});

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  EXPECT_TRUE(prog_is({2, 3, 0, 6, 99}));
}

TEST_F(OpCompTest, Day2TProg3) {
  load_prog({2, 4, 4, 5, 99, 0});

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  EXPECT_TRUE(prog_is({2, 4, 4, 5, 99, 9801}));
}

TEST_F(OpCompTest, Day2TProg4) {
  load_prog({1, 1, 1, 4, 99, 5, 6, 0, 99});

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  EXPECT_TRUE(prog_is({30, 1, 1, 4, 2, 5, 6, 0, 99}));
}

TEST_F(OpCompTest, ParamModes) {
  pmode_t param_modes;
  opc_t op;

  op = 1002;
  op = opcomp_param_modes(op, &param_modes);

  ASSERT_EQ(op, 2);
  ASSERT_EQ(P_MODE_FIRST(param_modes), 0);
  ASSERT_EQ(P_MODE_SECOND(param_modes), 1);
  ASSERT_EQ(P_MODE_THIRD(param_modes), 0);

  op = 11199;
  op = opcomp_param_modes(op, &param_modes);
  ASSERT_EQ(op, 99);
  ASSERT_EQ(P_MODE_FIRST(param_modes), 1);
  ASSERT_EQ(P_MODE_SECOND(param_modes), 1);
  ASSERT_EQ(P_MODE_THIRD(param_modes), 1);

  op = 1;
  op = opcomp_param_modes(op, &param_modes);
  ASSERT_EQ(op, 1);
  ASSERT_EQ(P_MODE_FIRST(param_modes), 0);
  ASSERT_EQ(P_MODE_SECOND(param_modes), 0);
  ASSERT_EQ(P_MODE_THIRD(param_modes), 0);

  op = 12399;
  op = opcomp_param_modes(op, &param_modes);
  ASSERT_EQ(op, 99);
  ASSERT_EQ(P_MODE_FIRST(param_modes), 3);
  ASSERT_EQ(P_MODE_SECOND(param_modes), 2);
  ASSERT_EQ(P_MODE_THIRD(param_modes), 1);
}

TEST_F(OpCompTest, RelativeMode) {
  load_prog({0, 3, 0, 2});

  oc.rbo = 3;
  int retval = opcomp_add(&oc, 0x21);

  ASSERT_EQ(retval, 0);
  ASSERT_EQ(oc.ip, 4);
  EXPECT_TRUE(prog_is({0, 3, 5, 2}));
}

TEST_F(OpCompTest, Day5TProg1) {
  load_prog({1002, 4, 3, 4, 33});

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  EXPECT_TRUE(prog_is({1002, 4, 3, 4, 99}));
}

TEST_F(OpCompTest, Day5TProg2) {
  load_prog({1101, 100, -1, 4, 0});

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  EXPECT_TRUE(prog_is({1101, 100, -1, 4, 99}));
}

TEST_F(OpCompTest, IOTest) {
  load_prog({3, 0, 4, 0, 99});

  opc_t input[1] = {13};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  EXPECT_TRUE(prog_is({13, 0, 4, 0, 99}));
  ASSERT_EQ(oc.output[0], 13);
}

/*
  Day5TProg1:
  Using position mode, consider whether the input is equal to 8; output 1 (if it
  is) or 0 (if it is not).
*/
TEST_F(OpCompTest, Day5TProg1_True) {
  load_prog({3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8});

  opc_t input[1] = {8};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1);
}

TEST_F(OpCompTest, Day5TProg1_False) {
  load_prog({3, 9, 8, 9, 10, 9, 4, 9, 99, -1, 8});

  opc_t input[1] = {4};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 0);
}

/*
  Day5TProg2:
  Using position mode, consider whether the input is less than 8; output 1 (if
  it is) or 0 (if it is not).
*/
TEST_F(OpCompTest, Day5TProg2_True) {
  load_prog({3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8});

  opc_t input[1] = {5};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1);
}

TEST_F(OpCompTest, Day5TProg2_False) {
  load_prog({3, 9, 7, 9, 10, 9, 4, 9, 99, -1, 8});

  opc_t input[1] = {9};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 0);
}

/*
  Day5TProg3:
  Using immediate mode, consider whether the input is equal to 8; output 1 (if
  it is) or 0 (if it is not).
*/
TEST_F(OpCompTest, Day5TProg3_True) {
  load_prog({3, 3, 1108, -1, 8, 3, 4, 3, 99});

  opc_t input[1] = {8};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1);
}

TEST_F(OpCompTest, Day5TProg3_False) {
  load_prog({3, 3, 1108, -1, 8, 3, 4, 3, 99});

  opc_t input[1] = {5};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 0);
}

/*
  Day5TProg4:
  Using immediate mode, consider whether the input is less than 8; output 1 (if
  it is) or 0 (if it is not).
*/
TEST_F(OpCompTest, Day5TProg4_True) {
  load_prog({3, 3, 1107, -1, 8, 3, 4, 3, 99});

  opc_t input[1] = {2};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1);
}

TEST_F(OpCompTest, Day5TProg4_False) {
  load_prog({3, 3, 1107, -1, 8, 3, 4, 3, 99});

  opc_t input[1] = {15};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 0);
}

/*
  Day5TProg5_True:
  Here are some jump tests that take an input, then output 0 if the input was
  zero or 1 if the input was non-zero: Position Mode.
*/
TEST_F(OpCompTest, Day5TProg5_True) {
  load_prog({3, 12, 6, 12, 15, 1, 13, 14, 13, 4, 13, 99, -1, 0, 1, 9});

  opc_t input[1] = {15};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1);
}

TEST_F(OpCompTest, Day5TProg5_False) {
  load_prog({3, 12, 6, 12, 15, 1, 13, 14, 13, 4, 13, 99, -1, 0, 1, 9});

  opc_t input[1] = {0};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 0);
}

/*
  Day5TProg6_True:
  Here are some jump tests that take an input, then output 0 if the input was
  zero or 1 if the input was non-zero: Immediate Mode.
*/
TEST_F(OpCompTest, Day5TProg6_True) {
  load_prog({3, 3, 1105, -1, 9, 1101, 0, 0, 12, 4, 12, 99, 1});

  opc_t input[1] = {15};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1);
}

TEST_F(OpCompTest, Day5TProg6_False) {
  load_prog({3, 3, 1105, -1, 9, 1101, 0, 0, 12, 4, 12, 99, 1});

  opc_t input[1] = {0};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 0);
}

/*
  Day5TProg7:
  The below example program uses an input instruction to ask for a single
  number. The program will then output 999 if the input value is below 8, output
  1000 if the input value is equal to 8, or output 1001 if the input value is
  greater than 8.
*/
TEST_F(OpCompTest, Day5TProg7_Below) {
  load_prog({3,    21,   1008, 21, 8,    20, 1005, 20,  22,  107,  8,    21,
             20,   1006, 20,   31, 1106, 0,  36,   98,  0,   0,    1002, 21,
             125,  20,   4,    20, 1105, 1,  46,   104, 999, 1105, 1,    46,
             1101, 1000, 1,    20, 4,    20, 1105, 1,   46,  98,   99});

  opc_t input[1] = {1};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 999);
}

TEST_F(OpCompTest, Day5TProg7_Eq) {
  load_prog({3,    21,   1008, 21, 8,    20, 1005, 20,  22,  107,  8,    21,
             20,   1006, 20,   31, 1106, 0,  36,   98,  0,   0,    1002, 21,
             125,  20,   4,    20, 1105, 1,  46,   104, 999, 1105, 1,    46,
             1101, 1000, 1,    20, 4,    20, 1105, 1,   46,  98,   99});

  opc_t input[1] = {8};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1000);
}

TEST_F(OpCompTest, Day5TProg7_Above) {
  load_prog({3,    21,   1008, 21, 8,    20, 1005, 20,  22,  107,  8,    21,
             20,   1006, 20,   31, 1106, 0,  36,   98,  0,   0,    1002, 21,
             125,  20,   4,    20, 1105, 1,  46,   104, 999, 1105, 1,    46,
             1101, 1000, 1,    20, 4,    20, 1105, 1,   46,  98,   99});

  opc_t input[1] = {10};
  opcomp_copy_to_input(&oc, input, 1);
  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1001);
}

TEST_F(OpCompTest, MaxSizeTest1) {
  load_prog({104,1125899906842624,99});

  opcomp_reserve_output(&oc, 1);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(oc.output[0], 1125899906842624);
}

/*
  Day9TProg1:
  Takes no input and produces a copy of itself as output.
*/
TEST_F(OpCompTest, Day9TProg1) {
  load_prog({109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99});

  opcomp_reserve_output(&oc, oc.plen);
  opcomp_reserve_memory(&oc, 1024 - oc.plen);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  EXPECT_TRUE(output_is({109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99}));
}

/*
  Day9TProg2:
  Should output a 16-digit number.
*/
TEST_F(OpCompTest, Day9TProg2) {
  load_prog({1102,34915192,34915192,7,4,7,99,0});

  opcomp_reserve_output(&oc, oc.plen);
  opcomp_reserve_memory(&oc, 1024 - oc.plen);

  int retval = opcomp_run(&oc);

  ASSERT_EQ(retval, -99);
  ASSERT_EQ(std::to_string(abs(oc.output[0])).length(), 16);
}

