/*
    (C) Copyright 2017 CEA LIST. All Rights Reserved.
    Contributor(s): Cingulata team (formerly Armadillo team)

    This software is governed by the CeCILL-C license under French law and
    abiding by the rules of distribution of free software.  You can  use,
    modify and/ or redistribute the software under the terms of the CeCILL-C
    license as circulated by CEA, CNRS and INRIA at the following URL
    "http://www.cecill.info".

    As a counterpart to the access to the source code and  rights to copy,
    modify and redistribute granted by the license, users are provided only
    with a limited warranty  and the software's author,  the holder of the
    economic rights,  and the successive licensors  have only  limited
    liability.

    The fact that you are presently reading this means that you have had
    knowledge of the CeCILL-C license and that you accept its terms.
*/

#include <vector>

/* local includes */
#include <bit_exec/tracker.hxx>
#include <ci_context.hxx>
#include <ci_fncs.hxx>
#include <ci_int.hxx>
#include <int_op_gen/mult_depth.hxx>

/* namespaces */
using namespace std;
using namespace cingulata;

#define SEX_FIELD 0
#define ANTECEDENT_FIELD 1

int main() {
  /* Set context to bit tracker and multiplicative depth minimized integer
   * operations */
  CiContext::set_config(make_shared<BitTracker>(), make_shared<IntOpGenDepth>());

  CiInt flags{0,2}; // 2 flags
  flags.read("flags");

  CiInt age{CiInt::u8}, cash{CiInt::u8}, saving{CiInt::u8}, debt{CiInt::u8},
      income{CiInt::u8}, realestate{CiInt::u8};

  age.read("age");
  cash.read("cash");
  saving.read("saving");
  debt.read("debt");
  income.read("income");
  realestate.read("realestate");

  vector<CiInt> keystream(7, CiInt::u8);
  // Read the pre-calculated keystream.
  for (int i = 0; i < 7; i++)
    keystream[i].read("ks_" + to_string(i));

  for (int i = 0; i < 2; i++)
    flags[i] ^= keystream[0][i];
  age ^= keystream[1];
  cash ^= keystream[2];
  saving ^= keystream[3];
  debt ^= keystream[4];
  income ^= keystream[5];
  realestate ^= keystream[6];

  vector<CiInt> credit_factors;

  //credit_factors.push_back(select(age < 55, 10, 0));
  //credit_factors.push_back(select((flags[SEX_FIELD]) && (age < 55), 10, 0));
  //credit_factors.push_back(select((!flags[SEX_FIELD]) && (age < 60), 10, 0));

  credit_factors.push_back(age);
  credit_factors.push_back(select((!flags[ANTECEDENT_FIELD]), 10, 0));

  credit_factors.push_back(cash);
  credit_factors.push_back(saving);
  credit_factors.push_back(-debt);
  //credit_factors.push_back(select((cash + saving - 5) > debt, cash + saving - debt, 0));
  for (int i = 0; i < 12; i++) {
    credit_factors.push_back(income);
  }
  credit_factors.push_back(select(realestate > 20, realestate, 0));

  CiInt credit = sum(credit_factors);

  credit.write("credit");

    /* Export to file the "tracked" circuit */
  CiContext::get_bit_exec_t<BitTracker>()->export_blif(blif_name, "rate");
}
