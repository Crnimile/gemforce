#ifndef _KGEXACT_UTILS_H
#define _KGEXACT_UTILS_H

struct Gem_YBp {
	int grade;              // short does NOT help
	float damage;           // this is MAX damage, with the rand() part neglected
	float crit;             // assumptions: crit chance capped
	float bbound;           // BB hit lv >> 1
	struct Gem_YBp* father; // maximize damage*bbound*crit*bbound
	struct Gem_YBp* mother;
	int place;
};

inline double gemP_power(gemP gem1)
{
	return gem1.damage*gem1.bbound*gem1.crit*gem1.bbound;
}

inline double gemP_bbd(gemP gem1)
{
	return gem1.bbound*gem1.bbound*gem1.damage;
}

inline double gemP_bbc(gemP gem1)
{
	return gem1.bbound*gem1.bbound*gem1.crit;
}

inline int gem_less_eq_exact(gemP gem1, gemP gem2)
{
	if (gem1.damage != gem2.damage)
		return gem1.damage<gem2.damage;
	if (gem1.bbound != gem2.bbound)
		return gem1.bbound<gem2.bbound;
	return gem1.crit<gem2.crit;
}

void ins_sort_exact (gemP* gems, int len)
{
	int i,j;
	gemP element;
	for (i=1; i<len; i++) {
		element=gems[i];
		for (j=i; j>0 && gem_less_eq_exact(element, gems[j-1]); j--) {
			gems[j]=gems[j-1];
		}
		gems[j]=element;
	}
}

void quick_sort_exact (gemP* gems, int len)
{
	if (len > 10)  {
		gemP pivot = gems[len/2];
		gemP* beg = gems;
		gemP* end = gems+len-1;
		while (beg <= end) {
			while (gem_less_eq_exact(*beg, pivot)) {
				beg++;
			}
			while (gem_less_eq_exact(pivot,*end)) {
				end--;
			}
			if (beg <= end) {
				gemP temp = *beg;
				*beg = *end;
				*end = temp;
				beg++;
				end--;
			}
		}
		if (end-gems+1 < gems-beg+len) {		// sort smaller first
			quick_sort_exact(gems, end-gems+1);
			quick_sort_exact(beg, gems-beg+len);
		}
		else {
			quick_sort_exact(beg, gems-beg+len);
			quick_sort_exact(gems, end-gems+1);
		}
	}
}

void gem_sort_exact (gemP* gems, int len)
{
	quick_sort_exact (gems, len);    // partially sort
	ins_sort_exact (gems, len);      // finish the nearly sorted array
}

inline int gem_less_crit(gemP gem1, gemP gem2)
{
	return gem1.crit<gem2.crit;
}

void ins_sort_crit (gemP* gems, int len)
{
	int i,j;
	gemP element;
	for (i=1; i<len; i++) {
		element=gems[i];
		for (j=i; j>0 && gem_less_crit(element, gems[j-1]); j--) {
			gems[j]=gems[j-1];
		}
		gems[j]=element;
	}
}

void quick_sort_crit (gemP* gems, int len)
{
	if (len > 10)  {
		gemP pivot = gems[len/2];
		gemP* beg = gems;
		gemP* end = gems+len-1;
		while (beg <= end) {
			while (gem_less_crit(*beg, pivot)) {
				beg++;
			}
			while (gem_less_crit(pivot,*end)) {
				end--;
			}
			if (beg <= end) {
				gemP temp = *beg;
				*beg = *end;
				*end = temp;
				beg++;
				end--;
			}
		}
		if (end-gems+1 < gems-beg+len) {		// sort smaller first
			quick_sort_crit(gems, end-gems+1);
			quick_sort_crit(beg, gems-beg+len);
		}
		else {
			quick_sort_crit(beg, gems-beg+len);
			quick_sort_crit(gems, end-gems+1);
		}
	}
}

void gem_sort_crit (gemP* gems, int len)
{
	quick_sort_crit (gems, len);    // partially sort
	ins_sort_crit (gems, len);      // finish the nearly sorted array
}

#endif // _KGEXACT_UTILS_H
