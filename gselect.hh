#ifndef __CPU_PRED_G_SELECT_PRED_HH__
#define __CPU_PRED_G_SELECT_PRED_HH__

class GSelectBP : public BPredUnit
{
  public:

    GSelectBP(const LocalBPParams *params);    
    
    bool lookup(ThreadID tid, Addr branch_addr, void * &bp_history);

    void update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history, bool squashed, const StaticInstPtr & inst, Addr corrTarget);
    
    virtual void uncondBranch(ThreadID tid, Addr pc, void * &bp_history);
    
    void btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history);   

    void squash(ThreadID tid, void *bp_history)
    { assert(bp_history == NULL); }

  private:

        struct BPHistory {
        unsigned globalHistoryReg;
        // was the taken array's prediction used?
        // true: takenPred used
        // false: notPred used
        bool takenUsed;
        // prediction of the taken array
        // true: predict taken
        // false: predict not-taken
        bool takenPred;
        // prediction of the not-taken array
        // true: predict taken
        // false: predict not-taken
        bool notTakenPred;
        // the final taken/not-taken prediction
        // true: predict taken
        // false: predict not-taken
        bool finalPred;
    };
    inline bool getPrediction(uint8_t &count);

    /** Calculates the local index based on the PC. */
    inline unsigned getLocalIndex(Addr &PC);

    /** Size of the local predictor. */
    const unsigned PredictorSize;

    /** Number of bits of the local predictor's counters. */
    const unsigned PHTCtrBits;
    
    /** Global history bits */
    const unsigned globalHistoryBits;

    /** Number of sets. */
    const unsigned localPredictorSets;

    /** Array of counters that make up the local predictor. */
    std::vector<SatCounter> localCtrs;

    /** Mask to get index bits. */
    const unsigned indexMask;
};

#endif // __CPU_PRED_G_SELECT_PRED_HH__
