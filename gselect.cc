#include "cpu/pred/gselect.hh"

#include "base/bitfield.hh"
#include "cpu/pred/gselect.hh"
#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/Fetch.hh"

GSelectBP::GSelectBP(const GSelectBPParams *params)
: BPredUnit(params),
globalHistoryBits(params->globalHistoryBits),
PredictorSize(params->PredictorSize),
PHTCtrBits(params->PHTCtrBits),
PHTCtr(PredictorSize, SatCounter(PHTCtrBits)), 
localPredictorSets(PredictorSize / PHTCtrBits),
localCtrs(localPredictorSets, SatCounter(PHTCtrBits)),
indexMask(localPredictorSets -1 ),
predictorIndexSize(ceilLog2(params->PredictorSize)), 
globalHistoryReg(params->numThreads, 0)
{
	if (!isPowerOf2(PredictorSize))
	{
		fatal("Invalid local predictor size!\n");
	}

	historyRegisterMask = mask(globalHistoryBits);
	DPRINTF(Fetch, "predictor size: %i\n", PredictorSize);
	DPRINTF(Fetch, "PHT counter bits: %i\n", PHTCtrBits);

	//Bits for branch address in index
	//branchAddBits = predictorIndexSize - globalHistoryBits;
	//M bit mask
	//gBranchMask = mask(branchAddBits);
	gBranchMask = mask ( predictorIndexSize - globalHistoryBits );
    	//N bit mask 
    	//historyRegistorMask = mask(globalHistoryBits);
    	//PHTCtrThreshold = (ULL(1) << (PHTCtrBits - 1)) - 1;
}

bool
GSelectBP::lookup(ThreadID tid, Addr branch_addr, void * &bp_history)
{

    //printf("entering lookup function");
    bool taken;
	
    //m bits from branch address
    unsigned mBits = lookupIndex(branch_addr);
    //n bits from global shift register
    unsigned nBits = globalHistoryReg[tid] & historyRegisterMask;
    //concatenate m bits by shifting n bits m to the left
    //nBits = nBits << branchAddBits;
    nBits = nBits << (predictorIndexSize - globalHistoryBits);
    // Add m to n
    unsigned predIndex = nBits + mBits;
    //Get taken or not taken 
    //see whether counter is less than or greater than half
    //taken = PHTCtrThreshold < PHTCtr[predIndex];
    taken = ((ULL(1) << (PHTCtrBits - 1)) - 1) < PHTCtr[predIndex];
    //New history backup 
    BPHistory *history = new BPHistory;
    history->globalHistoryReg = globalHistoryReg[tid];
    history->finalPrediction = taken;
    bp_history = static_cast<void*>(history);
    //updateGlobalHistReg(tid, taken);
	
    return taken;
}

unsigned
GSelectBP::lookupIndex(Addr branch_addr)
{
    // Gets rid of unnecessary bits
    return (branch_addr >> instShiftAmt) & (gBranchMask);

}

void GSelectBP::btbUpdate(ThreadID tid, Addr branchAddr, void * &bpHistory)
{

	globalHistoryReg[tid] &= (historyRegisterMask & ~ULL(1));
}

void GSelectBP::update(ThreadID tid, Addr branchAddr, bool taken, void *bpHistory, bool squashed, const StaticInstPtr & inst, Addr corrTarget)
{

//combined 2 bit select and bimode implementation
	//assert(bpHistory);
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

// bimode implementation (per class powerpoint)
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
	return (branch_addr >> 4) & gBranchMask;
	
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

GSelectBP* GSelectBPParams::create()
{
	return new GSelectBP(this);
}
