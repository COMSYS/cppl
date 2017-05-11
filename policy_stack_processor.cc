#include "policy_stack_processor.hh"

void PolicyStackProcessor::addStackOperation( PolicyStackProcessorNodeType type, bool value, int64_t relatedId){
	PolicyStackProcessorNode pspn;
	pspn.reason[0] = 0;
	pspn.reason[1] = 0;
	int64_t latestResult;
	int64_t secondLatestResult;

	switch(type){
		case PolicyStackProcessorNodeType::AND:
			latestResult = workStack.top();
			workStack.pop();
			secondLatestResult = workStack.top();
			workStack.pop();

			pspn.type = PolicyStackProcessorNodeType::AND;
			pspn.result = nodeList[latestResult].result && nodeList[secondLatestResult].result;

			if (pspn.result){
				pspn.reason[0] = secondLatestResult;
				pspn.reason[1] = latestResult;
			}
			else{
				if (nodeList[secondLatestResult].result == false)
					pspn.reason[0] = secondLatestResult;
				if (nodeList[latestResult].result == false){
					if (pspn.reason[0] == 0)
						pspn.reason[0] = latestResult;
					else
						pspn.reason[1] = latestResult;
				}
			}

			break;
		case PolicyStackProcessorNodeType::OR:
			latestResult = workStack.top();
			workStack.pop();
			secondLatestResult = workStack.top();
			workStack.pop();

			pspn.type = PolicyStackProcessorNodeType::OR;
			pspn.result = nodeList[latestResult].result || nodeList[secondLatestResult].result;

			if (pspn.result){
				if (nodeList[secondLatestResult].result == true)
					pspn.reason[0] = secondLatestResult;
				if (nodeList[latestResult].result == true){
					if (pspn.reason[0] == 0)
						pspn.reason[0] = latestResult;
					else
						pspn.reason[1] = latestResult;
				}
			}
			else{
				pspn.reason[0] = secondLatestResult;
				pspn.reason[1] = latestResult;
			}
			break;
		case PolicyStackProcessorNodeType::RELATION:
			pspn.type = PolicyStackProcessorNodeType::RELATION;
			pspn.result = value;
			pspn.reason[0] = relatedId;
			break;
	};
	nodeList.push_back(pspn);
	workStack.push(nodeList.size() - 1);
}

vector<PolicyStackProcessorNode> PolicyStackProcessor::getReason(){
	vector<PolicyStackProcessorNode> reasonTree;

	//push_back twice to make the index of the first element in reasonTree 1;
	reasonTree.push_back(nodeList[nodeList.size() - 1]);
	reasonTree.push_back(nodeList[nodeList.size() - 1]);
	/*for(vector<PolicyStackProcessorNode>::iterator it = reasonTree.begin()+1; it != reasonTree.end(); ++it){
		if (it->type == PolicyStackProcessorNodeType::RELATION){
			continue;
		}

		reasonTree.push_back(nodeList[it->reason[0]]);
		it->reason[0] = reasonTree.size() - 1;
		if (it->reason[1] != 0){
			reasonTree.push_back(nodeList[it->reason[1]]);
			it->reason[1] = reasonTree.size() - 1;
		}
	}*/
	for (unsigned long i = 1; i != reasonTree.size();++i){
		if (reasonTree[i].type == PolicyStackProcessorNodeType::RELATION){
			continue;
		}

		reasonTree.push_back(nodeList[reasonTree[i].reason[0]]);
		reasonTree[i].reason[0] = reasonTree.size() - 1;
		if (reasonTree[i].reason[1] != 0){
			reasonTree.push_back(nodeList[reasonTree[i].reason[1]]);
			reasonTree[i].reason[1] = reasonTree.size() - 1;
		}
	}

	return reasonTree;
}

void PolicyStackProcessor::printReason(ReasonPrinter & printer){
	printer.print(getResult(), getReason());
}