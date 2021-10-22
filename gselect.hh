#ifndef __CPU_PRED_G_SELECT_PRED_HH__
#define __CPU_PRED_G_SELECT_PRED_HH__

#include <vector>

#include "base/types.hh"
#include "base/sat_counter.hh"
#include "cpu/pred/bpred_unit.hh"
#include "params/GSelectBP.hh"

class GSelectBP : public BPredUnit
{
  public:

    GSelectBP(const GSelectBPParams *params);    
    
    bool lookup(ThreadID tid, Addr branch_addr, void * &bp_history);

    void update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history, bool squashed, const StaticInstPtr & inst, Addr corrTarget);
    
    void uncondBranch(ThreadID tid, Addr pc, void * &bp_history);
    
    void btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history);   

    void squash(ThreadID tid, void *bp_history);

  private:
  
  	//private variables and functions for bimode 
	void updateGlobalHistReg(ThreadID tid, bool taken);
	
        struct BPHistory {
        unsigned globalHistoryReg;
        // was the taken array's prediction used?
        // true: takenPred used
        // false: notPred used
        //might not need - bool takenUsed;
        // prediction of the taken array
        // true: predict taken
        // false: predict not-taken
        //might not need - bool takenPred;
        // prediction of the not-taken array
        // true: predict taken
        // false: predict not-taken
        //might not need - bool notTakenPred;
        // the final taken/not-taken prediction
        // true: predict taken
        // false: predict not-taken
        bool finalPrediction;
    };
    
 
    unsigned historyRegisterMask;
    
    //unsigned choicePredictorSize;
    //unsigned choiceCtrBits;
    //unsigned choiceHistoryMask;
    //unsigned globalPredictorSize;
    //unsigned globalCtrBits;
    //unsigned globalHistoryMask;
    
    //unsigned choiceThreshold;
    //unsigned takenThreshold;
    //unsigned notTakenThreshold;
    
    /** Array of counters that make up the bimode predictor. */
    std::vector<unsigned> globalHistoryReg;
    std::vector<SatCounter> choiceCounters;
    std::vector<SatCounter> takenCounters;
    std::vector<SatCounter> notTakenCounters;
    
//2-bit local functions and variables
    inline bool getPrediction(uint8_t &count);

    /** Calculates the local index based on the PC. */
    inline unsigned getLocalIndex(Addr &PC);

    /** Size of the local predictor. */
    const unsigned PredictorSize;

    /** Number of bits of the local predictor's counters. */
    const unsigned PHTCtrBits;
    
    unsigned globalHistoryBits;

    /** Number of sets. */
    const unsigned localPredictorSets;

    /** Array of counters that make up the local predictor. */
    std::vector<SatCounter> CounterCtrs;
 
    unsigned counterThreshold;
    
    /** Mask to get index bits. */
    const unsigned indexMask;
};

#endif // __CPU_PRED_G_SELECT_PRED_HH__
