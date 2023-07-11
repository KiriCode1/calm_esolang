/* Calm - reference implementation */

/*
   Made by Sergey Badikov
*/

#include <bitset>
#include <cstdint>
#include <deque>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <vector>

using namespace std;

struct curls
{
  	size_t p_span;
  	size_t c_span;
};

bool is_simple(const std::string& program, std::vector<curls> *c_output, std::string *p_output)
{
  std::string p;
  std::vector<curls> c;

  for (char cmd : program)
  {
      	if (cmd == '{' || cmd == '}' || cmd == '*' || cmd == '~' || cmd == '_')
	{
		p.push_back(cmd);
	}
  }

  std::stack<std::pair<size_t, size_t>> stck;

  if (!stck.empty())
  {
    	return false;
  }

  *c_output = std::move(c);
  *p_output = std::move(p);

  return true;
}

void run_code(const std::string& program, std::istream& input, std::ostream& output) 
{
  std::deque<std::bitset<64>> mem = {0};
  size_t pointer = 0;

  std::vector<curls> c_tmp;
  std::string p_tmp;

  if (!is_simple(program, &c_tmp, &p_tmp)) {
    	std::cerr << "Ubalanced braces occured\n";
    	
	return;
  }

  const std::string p = std::move(p_tmp);
  const std::vector<curls> c = std::move(c_tmp);

  uint8_t input_buffer = 0;
  size_t input_buffer_size = 0;
  uint8_t output_buffer = 0;
  size_t output_buffer_size = 0;
  bool input_failed = false;

  size_t ip = 0;
  size_t cp = 0;

  while (ip < p.size()) 
  {
    	switch (p[ip])
    	{
    	case '{': {
      		if (pointer == 0) 
      		{
        		mem.push_front(0);
        		pointer = 32;
      		} 
      		else 
      		{
        		pointer--;
      		} ip++;
   	 } break;

    	case '}': {
      		if (pointer == std::numeric_limits<size_t>::max()) 
      		{
        		throw std::runtime_error("Ran out of memory");
      		} pointer++;

      		if (pointer == mem.size() * 64)
      		{
        		mem.push_back(0);
      		} ip++;
    	} break;

    	case '~': {
      		if (input_buffer_size == 0)
      		{
        		if (!input_failed)
			{
          			char tmp;
          			if (!input.get(tmp))
	  			{
            				input_failed = true;
          			}
	  			else
	  			{
            				input_buffer = tmp;
          			}
        		} input_buffer_size = 8;
      		}
      	uint32_t bit = (input_buffer & 1);
      	input_buffer >>= 1;
      	input_buffer_size--;

      	mem[pointer / 64].set(pointer % 64, bit == 1);
      	ip++;
    	} break;

    	case '_': {
      		uint8_t bit = mem[pointer / 64][pointer % 64];

      		output_buffer |= (bit << output_buffer_size);
      		output_buffer_size++;

      		if (output_buffer_size == 8) 
      		{
        		output.put(output_buffer);
        		output_buffer = 0;
        		output_buffer_size = 0;
      		} ip++;
   	} break;

    	case '*': {
      		mem[pointer / 64].flip(pointer % 64);
      		ip++;
    	} break;
     }
  }

  if (output_buffer_size != 0) 
  {
    	output.put(output_buffer);
  }
}

int main(int argc, char **argv)
{
	if (argc != 2) 
  	{
    		std::cerr<<"Use only one argument.\n";

    		return 1;
  	}

  	std::ifstream f(argv[1]);

  	if (!f) 
  	{
    		std::cerr << "Error occured while opening file.\n";

    		return 1;
  	}

  	std::string program((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
  	run_code(program, std::cin, std::cout);
  
  	return 0;
}

