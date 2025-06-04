#ifndef TABLE_LOOKUP_H
#define TABLE_LOOKUP_H
#include <vector>
using std::vector;

class TableLookup {
	
	public:
	
		TableLookup() {
			
			// start from beginning of table by default
			phaseIndex_ = 0.f;
			
			table_ = nullptr;
			tableSize_ = 0;
		}
		inline void setTable(const vector<float>& table) {
			
			// store pointer to table
			table_ = &table;
			
			// store size of table (should be more
			// efficient than constantly dereferencing)
			tableSize_ = table_->size();
		}
		
	protected:
	
		const vector<float>* table_;
		unsigned tableSize_;
		float phaseIndex_;
		float lerp(float phase) const;
	
};
#endif // TABLE_LOOKUP_H