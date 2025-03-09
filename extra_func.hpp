#pragma once
#include <utility>
#include "bigint.h"
#include <random>
//primes and factors
namespace {
static int prime_bound=0;
std::vector<int> primes={2};

struct factorisation{
	//residue in form of bigint. Has prime factors bigger than 10^18 or prime itself
	bigint residue=0;

	std::vector<int> prime;
	std::vector<int> power;

	bigint to_bigint(){
		bigint ans=1;
		for(int i=0;i<prime.size();i++){
			bigint temp=prime[i];
			temp.to_pow(power[i]);
			ans*=temp;
		}
		return ans;
	}
	std::string to_string(){
		if(prime.size()==0){return "1";}
		std::string ans;
		for(int i=0;i<prime.size()-1;i++){
			ans+="("+std::to_string(prime[i])+"^"+std::to_string(power[i])+")*";
		}
		ans+="("+std::to_string(prime[prime.size()-1])+"^"+std::to_string(power[prime.size()-1])+")";
		if(residue.digits.size()>2){ans+="*"+residue.to_string()+"(may be prime or divisors>10^9)";} 
		else if(residue>0){ans+="*("+residue.to_string()+")";}
		return ans;
	}

};


void find_primes(){
prime_bound=1000000;
std::vector<char> is_prime(prime_bound,1);
for(int i=4;i<prime_bound;i+=2){
	is_prime[i]=0;
}
for(long long i=3;i<prime_bound;i++){
if(is_prime[i]){
	primes.push_back(i);
	for(long long j=i*i;j<prime_bound;j+=i*2){
		is_prime[j]=0;
	}
}
}
}

void find_primes(int n){
	prime_bound=n;
	std::vector<char> is_prime(prime_bound,1);
	for(int i=4;i<prime_bound;i+=2){
		is_prime[i]=0;
	}
	for(long long i=3;i<prime_bound;i++){
	if(is_prime[i]){
		primes.push_back(i);
		for(long long j=i*i;j<prime_bound;j+=i*2){
			is_prime[j]=0;
		}
	}
	}
	}

int search_in_primes(int n){
	if(prime_bound<n){find_primes((n>1000000)?1000000:n);}
if(n==2){return 0;}
if(n<2 || n>=prime_bound){
	return -1;
} else {
	int left=0;
	int right=primes.size()-1;
	int middle=(right+left)/2;
	while(right-left>1){
		if(primes[middle]==n){return middle;}
		else if(primes[middle]>n){
			right=middle;
			middle=(right+left)/2;
		} else {
			left=middle;
			middle=(right+left)/2;
		}
	}
	return right;
}
}

bool is_prime(int a){
	if(prime_bound<1000000){find_primes();}
	if(a<prime_bound){
	int x=search_in_primes(a);
	if(a==primes[x]){return true;}
	return false;
	} else {
	for(int i=0;i<4792;i++){
		if(a%primes[i]==0){return false;}
	}
	return true;
	}

	}


factorisation factorise(bigint a){
	if(prime_bound<1000000){find_primes();}
	factorisation ans;
	if(a==1){
		return ans;
	}
	for(int i=0;i<primes.size();i++){
		if(a%primes[i]==0){
		ans.prime.push_back(primes[i]);
		int power=1;
		a/=primes[i];
		while(a%primes[i]==0){
			power++;
			a/=primes[i];
		}
		ans.power.push_back(power);
		}
	if(a==1){return ans;}
	}
	if(a!=1){ans.residue=a;}
	return ans;
	}

}


//randomness
namespace {
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> rand10e9(0,1000000000); // random int <10^9
	std::uniform_int_distribution<std::mt19937::result_type> random_stop(0,100); // 
	
bigint random_bigint(){
bigint ans;
ans.digits.push_back(rand10e9(rng));
while(random_stop(rng)<90){
	ans.digits.push_back(rand10e9(rng));
}
return ans;
}

bigint random_bigint(int length){
	bigint ans;
	for(int i=0;i<length;i++){
		ans.digits.push_back(rand10e9(rng));
	}
	return ans;
	}
}

// Combinatorics
namespace {
	bigint factorial(int n){
		bigint ans=1;
		for(int i=1;i<=n;i++){
			ans*=i;
		}
		return ans;
	}

	//binomial
	bigint binomial(int n,int k){
		if(n-k<k){k=n-k;}
		bigint ans=1;
		for(int i=1;i<=k;i++){
			ans*=(n-i+1);
			ans/=i;
		}
		return ans;
	}
	
	//partial permutations
	bigint partperm(int n,int k){
		bigint ans=1;
		for(int i=k+1;i<=n;i++){
			ans*=i;
		}
		return ans;
	}
}

