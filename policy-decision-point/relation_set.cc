#include "relation_set.hh"

#ifdef __DEBUG__
#include <iostream>
#endif

void RelationSet::doEval(){
#ifdef __DEBUG__
	int i = 0;
#endif

	for (auto it = relation_list.begin(); it != relation_list.end(); ++it){
		const Variable * lv = variable_set.getVariable(it->relation->getLeftID());
		const Variable * rv = variable_set.getVariable(it->relation->getRightID());

		it->evalResult = it->relation->doEval(lv, rv);
#ifdef __DEBUG__
		if (it->evalResult)
			std::cout<<i<<std::endl;

		++i;
#endif
	}
}

id_type RelationSet::getRelationID(const Relation & r) const{
	id_type id = 0;
	for (;id != relation_list.size() && r != *relation_list[id].relation; ++id);

	if (id == relation_list.size())
		return std::numeric_limits<id_type>::max();
	else
		return id;
}
