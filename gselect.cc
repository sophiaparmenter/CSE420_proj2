#include "cpu/pred/gselect.hh"

#include "base/bitfield.hh"
#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/Fetch.hh"

GSelectBP::GSelectBP(const GSelectBPParams *params)
    : BPredUnit(params),
      globalHistoryBits(params->globalHistoryBits),
      PHTCtrBits(params->PHTCtrBits),
      PredictorSize(params->PredictorSize),
      localPredictorSets(PredictorSize / PHTCtrBits),
      CounterCtrs(localPredictorSets, SatCounter(PHTCtrBits)),
      indexMask(localPredictorSets -1 ) 
{

	if (!isPowerOf2(PredictorSize)){
		fatal("Invalid local predictor size!\n");
		}
	
	historyRegisterMask = mask(globalHistoryBits);
	DPRINTF(Fetch, "predictor size: %i\n", PredictorSize);
	DPRINTF(Fetch, "PHT counter bits: %i\n", PHTCtrBits);
     
      }
      
      
         
bool GSelectBP::lookup(ThreadID tid, Addr branchAddr, void * &bpHistory)
{

bool taken;

//m bits from the brach address
unsigned mBits = getLocalIndex(branchAddr);

//n bits from the global shift register
unsigned nBits = globalHistoryReg[tid] & historyRegisterMask;

//cocatenate m bits and shift n bits m to the left
nBits = nBits << PHTCtrBits;

//add m+n
unsigned predIndex = nBits + mBits;

//determine taken or not taken
//see whether counter is less than or greater than half
taken = counterThreshold < CounterCtrs[predIndex];

//update history
BPHistory *history = new BPHistory;
history->globalHistoryReg = globalHistoryReg[tid];
history->finalPrediction = taken;

bpHistory = static_cast<void*>(history);
updateGlobalHistReg(tid, taken);

return taken;


}

void GSelectBP::btbUpdate(ThreadID tid, Addr branchAddr, void * &bpHistory)
{

	//globalHistoryReg[tid] &= (historyRegisterMask & ~ULL(1));
}

void GSelectBP::update(ThreadID tid, Addr branchAddr, bool taken, void *bpHistory, bool squashed, const StaticInstPtr & inst, Addr corrTarget)
{

//combined 2 bit select and bimode implementation
	assert(bpHistory);
	BPHistory *history = static_cast<BPHistory*>(bpHistory);
	unsigned local_predictor_idx;	
	//no state to store and we do not update on the wrong path
	if (squashed){	
	//from bimode implementation
		//globalHistoryReg[tid] = (history->globalHistoryReg << 1) | taken;
		return;
		}
		
	local_predictor_idx = getLocalIndex(branchAddr);
	DPRINTF(Fetch, "Looking up index %#x\n", local_predictor_idx);
	
	if (taken){
		DPRINTF(Fetch, "Branch updated as taken.\n");
		//PHTCtrBits[local_predictor_idx]++;
		
		//from bimode implementation
		delete history;
		}
	else {
		DPRINTF(Fetch, "Branch updated as not taken.\n");
		//PHTCtrBits[local_predictor_idx]--;
		}		
}

void GSelectBP::uncondBranch(ThreadID tid, Addr pc, void * &bpHistory)
{

//2 bit select leaves this function empty

//bimode implementation (per class powerpoint)
	BPHistory *history = new BPHistory;
	//history->globalHistoryReg = globalHistoryReg[tid];
	bpHistory = static_cast<void*>(history);
	updateGlobalHistReg(tid, true);
}

void GSelectBP::squash(ThreadID tid, void *bpHistory)
{

//2-bit select leaves squash empty

//bimode implementation (from class powerpoint)
	BPHistory *history = static_cast<BPHistory*>(bpHistory);
	//history->globalHistoryReg[tid] = history->globalHistoryReg;
	delete history;

}

inline unsigned GSelectBP::getLocalIndex(Addr &branch_addr)
{
	return (branch_addr >> 4) & indexMask;
	
}

inline bool GSelectBP::getPrediction(uint8_t &count)
{
	//Get the MSB of the count
	return (count >> (PHTCtrBits -1));
}



void GSelectBP::updateGlobalHistReg(ThreadID tid, bool taken)
{
	globalHistoryReg[tid] = taken ? (globalHistoryReg[tid] << 1) | 1 :
					(globalHistoryReg[tid] << 1);
	globalHistoryReg[tid] &= historyRegisterMask;
	
}

GSelectBP* GSelectBPParams::create(){
	return new GSelectBP(this);
}

