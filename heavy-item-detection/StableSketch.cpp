#include "StableSketch.hpp"
#include <math.h>

StableSketch::StableSketch(int depth, int width, int lgn) {

	stable_.depth = depth;
	stable_.width = width;
	stable_.lgn = lgn;
	stable_.sum = 0;
	stable_.counts = new SBucket *[depth*width];
	for (int i = 0; i < depth*width; i++) {
		stable_.counts[i] = (SBucket*)calloc(1, sizeof(SBucket));
		memset(stable_.counts[i], 0, sizeof(SBucket));
		stable_.counts[i]->key[0] = '\0';
	}

	stable_.hash = new unsigned long[depth];
	stable_.scale = new unsigned long[depth];
	stable_.hardner = new unsigned long[depth];
	char name[] = "StableSketch";
	unsigned long seed = AwareHash((unsigned char*)name, strlen(name), 13091204281, 228204732751, 6620830889);
	for (int i = 0; i < depth; i++) {
		stable_.hash[i] = GenHashSeed(seed++);
	}
	for (int i = 0; i < depth; i++) {
		stable_.scale[i] = GenHashSeed(seed++);
	}
	for (int i = 0; i < depth; i++) {
		stable_.hardner[i] = GenHashSeed(seed++);
	}
}

StableSketch::~StableSketch() {
	for (int i = 0; i < stable_.depth*stable_.width; i++) {
		free(stable_.counts[i]);
	}
	delete[] stable_.hash;
	delete[] stable_.scale;
	delete[] stable_.hardner;
	delete[] stable_.counts;
}

void StableSketch::Update(unsigned char* key, val_tp val) {
	unsigned long bucket = 0;
	int keylen = stable_.lgn / 8;stable_.sum += 1;
	StableSketch::SBucket *sbucket;
	int flag = 0;
	long min = 99999999; int loc = -1; int k; int index; 
	for (int i = 0; i < stable_.depth; i++) {
		bucket = MurmurHash64A(key, keylen, stable_.hardner[i]) % stable_.width;
		index = i * stable_.width + bucket;
		sbucket = stable_.counts[index];
		if (sbucket->key[0] == '\0'&&sbucket->count==0) {
			memcpy(sbucket->key, key, keylen);
			flag = 1; 
			sbucket->count = 1;
            sbucket->stablecount = sbucket->stablecount+1; 
            return;
		}
		else if (memcmp(key, sbucket->key, keylen) == 0) { 
			{
              flag = 1;
			  sbucket->count += 1;
              sbucket->stablecount = sbucket->stablecount+1; 
            }
            return;
		}
        if (sbucket->count < min)
		{
			min = sbucket->count;
			loc = index; 
		}
	}
	if (flag == 0 && loc >= 0) 
	{
		sbucket = stable_.counts[loc]; 
		k = rand() % (int)((sbucket->stablecount*(sbucket->count))+1.0) + 1.0;
		if (k > (int)((((sbucket->count)*sbucket->stablecount)))) 
		{
			sbucket->count-=1;
			if(sbucket->count<=0)
			{memcpy(sbucket->key, key, keylen);
			sbucket->count+=1;
                        sbucket->stablecount = sbucket->stablecount - 1;
                        if(sbucket->stablecount <= 0)
                        {
                          sbucket->stablecount=0;
                        }}
		}
	}
}

void StableSketch::Query(val_tp thresh, std::vector<std::pair<key_tp, val_tp> >&results) {
	myset res;
	for (int i = 0; i < stable_.width*stable_.depth; i++) {                     
		if (stable_.counts[i]->count > (int)thresh) {
			key_tp reskey; 
			memcpy(reskey.key, stable_.counts[i]->key, stable_.lgn / 8);
			res.insert(reskey);
		}
	}
	for (auto it = res.begin(); it != res.end(); it++) {
		val_tp resval = 99999999; val_tp max = 0;
		for (int j = 0; j < stable_.depth; j++) {
			unsigned long bucket = MurmurHash64A((*it).key, stable_.lgn / 8, stable_.hardner[j]) % stable_.width;
			unsigned long index = j * stable_.width + bucket;
			if (memcmp(stable_.counts[index]->key, (*it).key, stable_.lgn / 8) == 0) {
				max += stable_.counts[index]->count;			
			}
			
		}
		if (max != 0)resval = max;
			key_tp key;
			memcpy(key.key, (*it).key, stable_.lgn / 8);
			std::pair<key_tp, val_tp> node;
			node.first = key;
			node.second = resval;
			results.push_back(node);
	}
	std::cout << "results.size = " << results.size() << std::endl;
}


val_tp StableSketch::PointQuery(unsigned char* key) {
	return Low_estimate(key);
}

val_tp StableSketch::Low_estimate(unsigned char* key) {


val_tp ret = 0, max = 0, min = 999999999;
	for (int i = 0; i < stable_.depth; i++) {
		unsigned long bucket = MurmurHash64A(key, stable_.lgn / 8, stable_.hardner[i]) % stable_.width;

		unsigned long index = i * stable_.width + bucket;
		if (memcmp(stable_.counts[index]->key, key, stable_.lgn / 8) == 0)
		{
			max += stable_.counts[index]->count;
		}
		index = i * stable_.width + (bucket + 1) % stable_.width;
		if (memcmp(key, stable_.counts[i]->key, stable_.lgn / 8) == 0)
		{
			max += stable_.counts[index]->count;
		}

	}
	return max;


}

val_tp StableSketch::Up_estimate(unsigned char* key) {

val_tp ret = 0, max = 0, min = 999999999;
	for (int i = 0; i < stable_.depth; i++) {
		unsigned long bucket = MurmurHash64A(key, stable_.lgn / 8, stable_.hardner[i]) % stable_.width;

		unsigned long index = i * stable_.width + bucket;
		if (memcmp(stable_.counts[index]->key, key, stable_.lgn / 8) == 0)
		{
			max += stable_.counts[index]->count;
		}
		if (stable_.counts[index]->count < min)min = stable_.counts[index]->count;
		index = i * stable_.width + (bucket + 1) % stable_.width;
		if (memcmp(key, stable_.counts[i]->key, stable_.lgn / 8) == 0)
		{
			max += stable_.counts[index]->count;
		}

	}
	if (max)return max;
	return min;

}

val_tp StableSketch::GetCount() {
	return stable_.sum;
}



void StableSketch::Reset() {
	stable_.sum = 0;
	for (int i = 0; i < stable_.depth*stable_.width; i++) {
		stable_.counts[i]->count = 0;
		memset(stable_.counts[i]->key, 0, stable_.lgn / 8);
	}
}

void StableSketch::SetBucket(int row, int column, val_tp sum, long count, unsigned char* key) {
	int index = row * stable_.width + column;
	stable_.counts[index]->count = count;
	memcpy(stable_.counts[index]->key, key, stable_.lgn / 8);
}

StableSketch::SBucket** StableSketch::GetTable() {
	return stable_.counts;
}






