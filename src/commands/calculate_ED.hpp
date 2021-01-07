#ifndef TEST_TEST_HPP
#define TEST_TEST_HPP

#include <math.h>
#include <vector>
#include <mockturtle/mockturtle.hpp>
#include <alice/alice.hpp>

namespace alice
{  
  class calculate_ED_command: public command
  {
      public:
      explicit calculate_ED_command( const environment::ptr& env ) : command( env, " Calculate the error distance " )
      {
	add_option( "--bit,-b", bit, " set the bit of adder " );
      }

      class print
      {
	public:
	    /*
	    void operator()(int val)
	    {
		std::cout << val << " ";
	    }
	    */
	    void operator()(kitty::dynamic_truth_table tt)
	    {
		kitty::print_hex(tt);
	    }
      };
	

      protected:
      void execute()
      {
	
	/* Get the approximate truth table from the "store<optimum_network>" */
	std::vector<kitty::dynamic_truth_table> adder_appro_tt;
	for(int i = 0; i < store<optimum_network>().size();i++)
	{
	    adder_appro_tt.push_back(store<optimum_network>()[i].function);
	    //kitty::print_hex(adder_appro_tt[i]);
	}


	/* Creates a combination of input truth tables to implement a configurable adder */
	std::vector<kitty::dynamic_truth_table> adder_input_tt;
	for(int i = 0;i < bit*2+1;i++)
	{
	    kitty::dynamic_truth_table tt(bit*2+1);
	    kitty::create_nth_var(tt,i);
	    adder_input_tt.push_back(tt);
	}
	
	
	kitty::dynamic_truth_table s_tt(bit*2+1);
	kitty::dynamic_truth_table cin_tt(bit*2+1);
	cin_tt = adder_input_tt[bit*2];

	/* Exact configurable adder truth table */
	std::vector<kitty::dynamic_truth_table> adder_exact_tt;
	for(int i = 0; i < bit;i++)
	{
	    s_tt = kitty::ternary_xor(adder_input_tt[i],adder_input_tt[bit+i],cin_tt);
	    adder_exact_tt.push_back(s_tt);
	    cin_tt = kitty::ternary_majority(adder_input_tt[i],adder_input_tt[bit+i],cin_tt);
	}
	adder_exact_tt.push_back(cin_tt);

	
	/* get the decimal value of exact truth table */
	int size_tt = pow(2,2*bit+1);
	int exact_tt_decimal[size_tt] = {0};
	for(int i = 0;i < size_tt;i++)
	{
	    for(int j = 0,weight = 0;j < bit+1;j++)
	    {
		weight = pow(2,j);
		exact_tt_decimal[i] += get_bit(adder_exact_tt[j],i) * weight;
	    }
	}

	/* get the decimal value of approximate truth table*/	
	int appro_tt_decimal[size_tt] = {0};
	for(int i = 0;i < size_tt;i++)
	{
	    for(int j = 0,weight = 0;j < bit+1;j++)
	    {
		weight = pow(2,j);
		appro_tt_decimal[i] += get_bit(adder_appro_tt[j],i) * weight;
	    }
	}
	
	/* calculate the error distance */
	int ed = 0,mae = 0;
	for(int i = 0,temp = 0;i < size_tt;i++)
	{
	    temp = abs(exact_tt_decimal[i] - appro_tt_decimal[i]);
	    ed += temp;
	    mae = temp > mae? temp:mae;
	}

	std::cout << " the Error Distance is " << ed << std::endl;
	std::cout << " the MAE is " << mae << std::endl;
	
	double sum_weight = 0;
	for(int i = 0;i < bit+1;i++)
	{
	    sum_weight += (double)pow(2,i);
	}

	std::cout << " the NMED is " << (double)ed/(double)size_tt/sum_weight << std::endl;
	
	#if 0
	/* for debug */
	for(int i = 0; i < size_tt;i++)
	{
	    std::cout << exact_tt_decimal[i] << " "; 
	}
	std::cout << std::endl;

	for(int i = 0;i < size_tt;i++)
	{
	    std::cout << appro_tt_decimal[i] << " ";
	}
	std::cout << std::endl;
	
	
	/* for debug */
	for(int i = 0; i < adder_input_tt.size();i++)
	{
	    kitty::print_hex(adder_input_tt[i]);
	    std::cout << std::endl;

	}

	for(int i = 0;i < adder_exact_tt.size();i++)
	{
	    kitty::print_hex(adder_exact_tt[i]);
	    std::cout << std::endl;
	}
	#endif
	    
      }
    
      private:
      int bit;
  };

  ALICE_ADD_COMMAND( calculate_ED, "Various" )
}

#endif
