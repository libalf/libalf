#include <libalf/algorithm_dfa_inferring_minisat.h>
#include <libalf/knowledgebase.h>
#include <libalf/alphabet.h>
#include <list>

using namespace libalf;

int main(int argc, char**argv)
{
	//data fields
	int alphabet_size = 2;
	bool symmetry_breaking = true;
	bool log_model = true;

	dfa_inferring_MiniSat infer_alg(NULL, NULL, alphabet_size);
	infer_alg.set_symmetry_breaking(symmetry_breaking);
	infer_alg.set_log_model(log_model);
	
	//serialize
	std::cout << "serializing...";
	std::basic_string<int32_t> serial = infer_alg.serialize();
	std::cout << "done." << std::endl;

	//deserialize
	std::cout << "deserializing...";
	dfa_inferring_MiniSat infer_alg2(NULL, NULL, 0);
	serial_stretch  ss(serial);	
	if(infer_alg2.deserialize(ss))
		std::cout << "done.";
	else
		std::cout << "failed.";
	std::cout << std::endl << std::endl;

	std::cout << "CHECK: alphabet_size --- " << (infer_alg2.get_alphabet_size() == alphabet_size? "OK": "ERROR") << std::endl;
	std::cout << "CHECK: symmetry_breaking --- " << (infer_alg2.uses_symmetry_breaking() == symmetry_breaking? "OK": "ERROR") << std::endl;
	std::cout << "CHECK: model logging --- " << (infer_alg2.is_logging_model() == log_model? "OK": "ERROR") << std::endl;

	std::cout << std::endl;
	return 0;
}

