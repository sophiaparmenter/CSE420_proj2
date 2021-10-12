#ifndef __CPU_PRED_G_SELECT_PRED_HH__
#define __CPU_PRED_G_SELECT_PRED_HH__

class GSelectBP : public BPredUnit
{
  public:
    /**
     * Default branch predictor constructor.
     */
    GSelectBP(const LocalBPParams *params);    

    /**
     * Looks up the given address in the branch predictor and returns
     * a true/false value as to whether it is taken.
     * @param branch_addr The address of the branch to look up.
     * @param bp_history Pointer to any bp history state.
     * @return Whether or not the branch is taken.
     */
    bool lookup(ThreadID tid, Addr branch_addr, void * &bp_history);

     /**
     * Updates the branch predictor with the actual result of a branch.
     * @param branch_addr The address of the branch to update.
     * @param taken Whether or not the branch was taken.
     */
    void update(ThreadID tid, Addr branch_addr, bool taken, void *bp_history, bool squashed, const StaticInstPtr & inst, Addr corrTarget);
    
    virtual void uncondBranch(ThreadID tid, Addr pc, void * &bp_history);
    /**
     * Updates the branch predictor to Not Taken if a BTB entry is
     * invalid or not found.
     * @param branch_addr The address of the branch to look up.
     * @param bp_history Pointer to any bp history state.
     * @return Whether or not the branch is taken.
     */
    void btbUpdate(ThreadID tid, Addr branch_addr, void * &bp_history);   

    void squash(ThreadID tid, void *bp_history)
    { assert(bp_history == NULL); }

  private:
    /**
     *  Returns the taken/not taken prediction given the value of the
     *  counter.
     *  @param count The value of the counter.
     *  @return The prediction based on the counter value.
     */
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
