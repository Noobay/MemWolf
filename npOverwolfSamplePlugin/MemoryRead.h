int InitReadMemory(const char* windowClass,const char* caption, double addressOffset);
bool isLootAvailable();
bool FinishReadingMemory();
template<typename _ret_t> _ret_t ReadMemory();
