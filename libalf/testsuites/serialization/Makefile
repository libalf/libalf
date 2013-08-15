infer_det_csp_minisat:
	g++ -w -I../../include -fopenmp -o serialization_infer_det_csp_minisat serialization_infer_det_csp_minisat.cpp

infer_det_csp_z3:
	g++ -w -I../../include -fopenmp -o serialization_infer_det_csp_z3 serialization_infer_det_csp_z3.cpp -lz3

infer_det_minisat:
	g++ -w -I../../include -fopenmp -o serialization_infer_det_minisat serialization_infer_det_minisat.cpp -lalf

infer_det_z3:
	g++ -w -I../../include -fopenmp -o serialization_infer_det_z3 serialization_infer_det_z3.cpp -lz3 -lalf

infer_dfa_minisat:
	g++ -w -I../../include -fopenmp -o serialization_infer_dfa_minisat serialization_infer_dfa_minisat.cpp -lalf

infer_dfa_z3:
	g++ -w -I../../include -fopenmp -o serialization_infer_dfa_z3 serialization_infer_dfa_z3.cpp -lz3 -lalf

infer_nfa_minisat:
	g++ -w -I../../include -fopenmp -o serialization_infer_nfa_minisat serialization_infer_nfa_minisat.cpp -lalf

counter_example_learning:
	g++ -g -w -I../../include -fopenmp -o serialization_counter_example_learning serialization_counter_example_learning.cpp -lalf -lz3

kv:
	g++ -g -w -I../../include -fopenmp -o serialization_kearns_vazirani serialization_kearns_vazirani.cpp -lalf -lAMoRE++

clean:
	-rm serialization_infer_det_csp_minisat serialization_infer_det_csp_z3 serialization_infer_det_minisat serialization_infer_det_z3  serialization_infer_dfa_minisat serialization_infer_dfa_z3 serialization_infer_nfa_minisat serialization_counter_example_learning 
