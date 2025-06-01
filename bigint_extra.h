#pragma once
#include <utility>
#include "bigint.h"
#include <random>


//randomness
std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> rand10e9(0,1000000000); // random int <10^9
std::uniform_int_distribution<std::mt19937::result_type> random_stop(0,100); // 

static int cur_random=0;

bigint random_bigint(){
bigint ans;
ans.digits.push_back(rand10e9(rng));
while(random_stop(rng)<90){
ans.digits.push_back(rand10e9(rng));
}
ans.trim();
return ans;
}

bigint random_bigint(int length){
bigint ans;
for(int i=0;i<length-1;i++){
	ans.digits.push_back(rand10e9(rng));
}

return ans;
}




//primes and factors	
std::vector<int> very_small_primes={2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
static int prime_bound=0;
std::vector<int> primes={2};


//takes ~10ms, all primes under 1000000
void find_primes_mil(){
if(prime_bound>=1000000){return;}
prime_bound=1000000;
std::vector<char> is_prime(prime_bound,1);
primes.reserve(78500);

for(long long i=3;i<1000;i+=2){
if(is_prime[i]){
	primes.push_back(i);
	for(long long j=i*i;j<prime_bound;j+=i*2){
		is_prime[j]=0;
	}
}
}
for(long long i=1001;i<prime_bound;i+=2){
	if(is_prime[i]){
		primes.push_back(i);
		
	}
}
}

//euclid till border
void find_primes(int n){
	if(prime_bound<n){
	prime_bound=n;
	int switch_border=sqrt(n);
	if(switch_border%2==0){
		switch_border--;
	}
	std::vector<char> is_prime(prime_bound,1);
	for(long long i=3;i<switch_border;i+=2){
	if(is_prime[i]){
		primes.push_back(i);
		for(long long j=i*i;j<prime_bound;j+=i*2){
			is_prime[j]=0;
		}
	}
	}
	for(long long i=switch_border;i<prime_bound;i+=2){
		if(is_prime[i]){
			primes.push_back(i);
			for(long long j=i*i;j<prime_bound;j+=i*2){
				is_prime[j]=0;
			}
		}
		}
	}
}

//all primes in int. Takes ~ 15s
void find_primes_int(){
	if(prime_bound<2147483647){
	prime_bound=2147483647;
	std::vector<char> is_prime(prime_bound-1,1);
	primes.reserve(105097565);
	for(long long i=3;i<46341;i+=2){
	if(is_prime[i]){
		if(i>primes.back()){
		primes.push_back(i);
		}
		for(long long j=i*i;j<prime_bound-1;j+=i*2){
			is_prime[j]=0;
		}
	}
	}
	for(long long i=46341;i<prime_bound-1;i+=2){
		if(is_prime[i] && i>primes.back()){
			primes.push_back(i);
		}
		}
	primes.push_back(2147483647);
}
}

//counts how many primes are less than n. binary search euclid
int prime_count(int n){
	if(prime_bound<n){find_primes((n>1000000)?1000000:n);}
if(n==2){return 1;}
if(n<2 || n>=prime_bound){
	return -1;
} else {
	int left=0;
	int right=primes.size()-1;
	int middle=(right+left)/2;
	while(right-left>1){
		if(primes[middle]==n){return middle+1;}
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


int32_t pow_mod(int64_t n, int32_t power, int64_t m) {
    int64_t result = 1;

    while (power) {
        if (power & 1) result = (result * n) % m;

        n = (n * n) % m;

        power >>= 1;
    }

    return (int32_t) result;
}

int32_t mul_mod(int32_t a,int32_t b,int32_t m){
	return (int32_t) (((int64_t) a * b) % m);
}
//Тест Миллера-Раббина для int, детерминированный. Используем a= 2, 3, 5. Исключаем 4 псевдопростых из диапазона (1,2^31-1)
bool MillerRabbin(int32_t n) {
    if ((n & 1) == 0) return n == 2;
    if (n < 9) return n > 1; // 3, 5 и 7.

    int32_t s = __builtin_ctz(n - 1);
    int32_t t = (n - 1) >> s;

    int32_t primes[3] = {2, 3, 5};

    for (int32_t a : primes) {
        int32_t x = pow_mod(a, t, n);

        if (x == 1) continue;

        for (int i = 1; x != n - 1; i++) {
            if (i == s) return false;

            x = mul_mod(x, x, n);

            if (x == 1) return false;
        }
    }

	//Псевдопростые для a =2,3,5
    switch (n) {
        case 25326001:
        case 161304001:
        case 960946321:
        case 1157839381:
            return false;

        default:
            return true;
    }
}

//MillerRabbin for bigint
bool MillerRabbin(const bigint& n,int rounds){
bigint n1=n-1;
if(n.even()){return false;}
if(n.digits.size()==1){return MillerRabbin(n.digits[0]);}
bigint t=n1;
long long s=0;
s=t.shift_to_odd();
next:
while(rounds){
	--rounds;
	bigint a=pow_mod(random_bigint(n.digits.size()-1)+1,t,n);
	if(a==1 || a==n1){continue;}
	for(int i=0;i<s-1;i++){
		a=a*a%n;
		if(a==1){return false;}
		if(a==n1){goto next;}
	}
	return false;
}
return true;
}

//MillerRabbin for bigint, default numbers preset of rounds
bool MillerRabbin(const bigint& n){
	if(n==2){return true;}
	if(n.even()){return false;}
	if(n.digits.size()==1){return MillerRabbin(n.digits[0]);}
	std::vector<bigint> preset;
	preset.push_back(bigint(2));
	preset.push_back(bigint(3));
	preset.push_back(bigint(5));
	preset.push_back(n/2);
	preset.push_back(n>>2);
	for(int i=0;i<5;i++){preset.push_back(random_bigint(n.digits.size()-1)+1);}
	int rounds=preset.size();
	bigint n1=n-1;
	bigint t=n1;
	long long s=0;
	s=t.shift_to_odd();
	next:
	while(rounds){
		--rounds;
		bigint a=pow_mod(preset[rounds],t,n);
		if(a==1 || a==n1){continue;}
		for(int i=0;i<s-1;i++){
			a=a*a%n;
			if(a==1){return false;}
			if(a==n1){goto next;}
		}
		return false;
	}
	return true;
	}


bool test_if_prime(bigint a){
	int small_primes[15]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47};
	
	for(int i=0;i<15;i++){
		if(a%small_primes[i]==0){
			if(a==small_primes[i]){return true;}
			else {return false;}
		}
	}
	return MillerRabbin(a);
}


bigint random_prime_bigint(){
	find_primes_mil();
	bigint ans=random_bigint()*2310+primes[rand10e9(rng)%343+5];
	while(!MillerRabbin(ans)){
		ans=random_bigint()*2310+primes[rand10e9(rng)%343+5];
	}
	return ans;
}

bigint random_prime_bigint(int n){
	bigint ans=random_bigint(n)*2310+primes[rand10e9(rng)%343+5];
	while(!test_if_prime(ans)){
		ans=random_bigint(n)*2310+primes[rand10e9(rng)%343+5];
	}
	return ans;
}

struct factorisation{
	std::vector<std::pair<bigint,long long>> list;
	bigint residue;
	bool unfinished=false;

	void print(){
		if(list.size()==0){
			std::cout<<"1";
		} else {
			std::cout<<list[0].first;
			if(list[0].second>1){
				std::cout<<" ^ "<<list[0].second;
			}
			for(int i=1;i<list.size();i++){
				std::cout<<" * "<<list[i].first;
				if(list[i].second>1){
					std::cout<<"^"<<list[i].second;
				}
			}
			if(unfinished){std::cout<<" * "<<residue<<" (not prime)";}
		}
		std::cout<<"\n";
	}	

};



void pollard_rho(bigint a,factorisation& where){
	if(MillerRabbin(a)){
		where.list.push_back(std::make_pair(a,1));
	} else {
		int tries=0;
		while(tries<10){
		again:
		tries++;
		int stage=0;
		bigint x=2;
		bigint y=2;
		bigint d;
		while(stage<100){
			x=(x*x+1)%a;
			y=(y*y+1)%a;
			y=(y*y+1)%a;
			if(x>y){
				d=x-y;
			} else {
				d=y-x;
			}
			d=gcd(d,a);
			if(d==a){goto again;}
			if(d>1){
				pollard_rho(d,where);
				pollard_rho(a/d,where);
			}
			stage++;
		}
		}
	}
}

//simple factorisation using existing primes
factorisation factorise(bigint a){
	if(prime_bound<1000000){find_primes_mil();}
	factorisation ans;
	if(a==1){
		return ans;
	}
	if(test_if_prime(a)){
		ans.list.push_back(std::make_pair(a,1));
		return ans;
	}
	for(int i=0;i<primes.size()&&i<78498;i++){
		if(a%primes[i]==0){
			long long power=0;
			while(a%primes[i]==0){
				power++;
				a/=primes[i];
			}
			ans.list.push_back(std::make_pair(bigint(primes[i]),power));
			if(a==1){return ans;}
		}
	}
	if(MillerRabbin(a)){
		ans.list.push_back(std::make_pair(a,1));
		return ans;
	}

	if(a!=1){pollard_rho(a,ans);}
	return ans;
}

// Тест простоты Ферма
bool Fermat(const bigint& n,int tries){
for(;tries;--tries){
	if(pow_mod((n-rand10e9(rng)*rand10e9(rng)-1),n-1,n)!=1) return false;
}
return true;
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
	
	//fibonacci
	bigint fibonacci(int n){
		int small_fibs[]={0,1,1,2,3,5,8,13,21,34,55};
		if(n<=10){
			return bigint(small_fibs[n]);
		}
		std::vector<char> digits;
		while(n){
			digits.push_back(n%2);
			n/=2;
		}
		bigint a=1;
		bigint b=1;
		bigint c=0;
		for(int i=digits.size()-2;i>=0;i--){
			a*=a;
			b*=b;
			c*=c;
			a+=b;
			c+=b;
			b=a-c;
			if(digits[i]){
				c=b;
				b=a;
				a+=c;
			}
		}
		return b;
	}
}

