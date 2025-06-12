#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "bigint.h"
#include "bigint_extra.h"
#include <algorithm>
#include "expression.h"
#include "evaluate.hpp"

bool primes_int_initialised=false;
std::string CURRENT_VERSION="12.06.2025";


void print_factor(bigint a){
    if(a==0){
        std::cout<<"zero"<<"\n";
        return;
    }
    if(a==1){
        std::cout<<a<<"\n";
        return; 
    }
    if(test_if_prime(a)){
        std::cout<<a<<" is prime\n";
        return;
    }
    std::cout<<a<<" = ";
    bool first=true;
    for(int i=0;i<primes.size() && a>1;i++){
        if(a%primes[i]==0){
            int pow=0;
            while(a%primes[i]==0){
                a/=primes[i];
                ++pow;
            }
            if(first){
                std::cout<< primes[i];
                first=false;
            } else {
                std::cout<<" * "<<primes[i];
            }
            if(pow>1){
                std::cout<<"^"<<pow;
            }
        if(i>78498){
            if(test_if_prime(a)){
                std::cout<<" * "<<a;
                return;
            } else {
                std::cout<<" * "<<a<< "(not prime, has divisors >10^6)";
                return;
            }
        }
        }
    }
    if(a>1){
        if(first){}else{
        std::cout<<" * "<<a;
        if(!test_if_prime(a)){std::cout<<"(not prime,has divisors";
        if(primes_int_initialised){std::cout<<" over 2^32)";} else {std::cout<<" over 10^9)";}}
        }
    }
    std::cout<<"\n";
}

int sum_digits(bigint a){
    int ans=0;
    for(int i=0;i<a.digits.size();i++){
        int tempo=a.digits[i];
        while(tempo){
            ans+=tempo%10;
            tempo/=10;
        }
    }
    return ans;
}

std::string convert_base(int old_base,int new_base,std::string number)
{   
    const std::string digits={"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    bigint NUMBER=0;
    std::string ans;
    bigint q=1;
    for(int i=number.length()-1;i>=0;i--){
        if((int)number[i]>64){
        NUMBER+=q*((int)number[i]-55);
        } else {
        NUMBER+=q*((int)number[i]-48);
        }
        q*=old_base;
    }
    while(NUMBER.isPositive()){
    ans+=digits[NUMBER%new_base];
    NUMBER/=new_base;
    }
    std::reverse(ans.begin(),ans.end());
    return ans;
}

//enum of all available actions
enum actions{
    skip=1,
    quit,
    clear,
    reset,
    help,
    show_number,
    change_sign,
    plus,
    minus,
    multi,
    divide_int,
    modulo,
    to_pow_bigint,
    sqr,
    intsqrt,
    to_powmod_bigint,
    is_prime,
    is_prime_pow,
    factor,
    factorl,
    GCD,
    random_big_prime,
    random_small_prime,
    nthprime,
    init,
    bin,
    sum_of_digits,
    number_of_digits,
    count_primes_before,
    base_to_base,
    hextobin,
    bintohex,
    hextodec,
    dectohex,
    bintodec,
    dectobin,
    gcdall,
    demoprimes,
    demofib,
    fib,
    primorial,
    EGCD,
    MODULAR_INVERSE,
    EVAL
};

//Maps actions to strings
std::map<std::string,actions> what_to_do{{"show",show_number},{"reset",reset},{"eval",EVAL},{"inverse",MODULAR_INVERSE},{"egcd",EGCD},{"nod",number_of_digits},{"countdigits",number_of_digits},{"primorial",primorial},{"demofib",demofib},{"fib",fib},
{"primes",demoprimes},{"powmod",to_powmod_bigint},
{"gcdall",gcdall},{"bintohex",bintohex},{"bintodec",bintodec},
{"dectohex",dectohex},{"dectobin",dectobin},{"hextodec",hextodec},{"hextobin",hextobin},
{"base",base_to_base},{"pi",count_primes_before},{"pow",to_pow_bigint},{"sod",sum_of_digits},
{"bin",bin},{"binomial",bin},{"gcd",GCD},{"factorial",factorl},{"factor",factor},{"q",quit},
{"help",help},{"is",skip},{"quit",quit},{"leave",quit},{"q",quit},{"clear",clear},{"c",clear},
{"prime?",is_prime},{"prime",is_prime},{"rbp",random_big_prime},{"rsp",random_small_prime},{"pn",nthprime},{"init",init}};

void print_help(){
            std::cout<<"Version "<<CURRENT_VERSION<<"\n";
            std::cout<<"Format is: command argument_1 argument_2 ...\n";
            std::cout<<"List of commands:\n";
            std::cout<<"init -- euclid for all integers. saved in primes\n";
            //primes and factors
            std::cout<<"rsp -- random small prime, under 1000000\n";
            std::cout<<"rbp -- random big prime of size 9*[n]\n";
            std::cout<<"pn -- prime number [n] \n";
            std::cout<<"pi -- how many primes before [n]. Gotta init for [n]>1000000 \n";
            std::cout<<"prime -- tells if a number is prime \n";
            std::cout<<"primes -- first [n] primes";
            std::cout<<"factor -- factorisation\n";
            std::cout<<"gcd -- finds gcd of 2 large number [a] [b]\n";
            std::cout<<"egcd -- extended gcd [a] [b]\n";
            //combinatorics
            std::cout<<"binomial-- finds binomial coefficient [n] [m] \n";
            std::cout<<"factorial -- [n]!\n"; 
            std::cout<<"fib -- fibonacci number [n]\n";
            std::cout<<"demofib -- first few fibonacci nums\n";
            //digits 
            std::cout<<"sod -- sum of digits [n]\n";
            std::cout<<"nod -- number of digits [n]\n";
            std::cout<<"base -- converts from [OLD_BASE] to [NEW_BASE NUMBER] [n] \n";
            std::cout<<"hextodec,hextobin,dectobin,dectohex,bintohex,bintodec -- base conversions [n] \n";
            //modular 
            std::cout<<"pow -- to power [n] [power]\n";
            std::cout<<"powmod -- to power mod [n] [power] [mod]\n";
            std::cout<<"inverse -- modular inverse [n] [mod]\n";

            //eval
            std::cout<<"eval -- (testing) enter a comprehansive evaluation mode.\n Can evaluate both big integers and doubles. \n For a simple int calculator just write stuff\n";
}

 int main(){ 
    int temp;
    system("cls");
    sieve_mil();
    bool is_runnig=true;
    bool no_number=true;
    std::string input;
    bigint current_number=0;
    bigint temp_number;
    again:
    while(is_runnig){
        std::cin>>input;
        if(input.length()==0){
            goto again;
        }
        //evaluates expressions, if no commands are inputed
        if(input[0]=='+' || input[0]=='-' || input[0]=='*' || input[0]=='/' || input[0]=='%' || input[0]=='^' ){
            if(current_number.isNegative()){
                input="(0"+current_number.to_string()+")"+input;
            } else {input=current_number.to_string()+input;
            }
            try {
            auto rpn = toRPN(input);
            current_number = evaluateRPN(rpn);
            std::cout << current_number<<"\n";
            }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        goto again;
        }
        if(input[0]=='('|| input[0]>47 && input[0]<58){
            {
            try {
            auto rpn = toRPN(input);
            current_number = evaluateRPN(rpn);
            std::cout << current_number<<"\n";
            }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        }
        goto again;
        }
        actions action=what_to_do[input];
        after_input:
        switch (action)
        {
        case skip:
        break;
        case show_number:
        std::cout<<current_number<<"\n";
        break;
        case EVAL:
        {
            evl::calculator_mode();
            break;
        }
        case help:
            print_help();
            break;
        case quit:
            is_runnig=false;
            break;
        case clear:
            system("cls");
            break;
        case reset:
            no_number=true;
            current_number=0;
            break;
        case init:

            sieve_int();
        
            primes_int_initialised=true;
            std::cout<<"found!\n";
            break;
        case factor:
            std::cin>>current_number;
            print_factor(current_number);
            break;
        case demoprimes:
            std::cin>>temp;
            for(int i=0;i<temp;i++){
                std::cout<<primes[i]<<" ";
            }
            std::cout<<"\n";
            break;
        case demofib:{
            long long a=1;
            long long b=1;
            long long c=0;
            for(int i=0;i<40;i++){
                std::cout<<c<<" ";
                c=b;
                b=a;
                a+=c;
            }   
            std::cout<<"\n";
            break;
        }
        case is_prime:
            std::cin>>current_number;
            std::cout<<(test_if_prime(current_number)?"prime":"not prime")<<"\n";
            break;
        case GCD:
            std::cin>>current_number;
            std::cin>>temp_number;
            current_number=gcd(current_number,temp_number);
            std::cout<<current_number<<"\n";
            break;
        case EGCD:{
        std::cin>>current_number;
        std::cin>>temp_number;
        std::vector<bigint> ans=egcd(current_number,temp_number);
        std::cout<<"gcd("<<current_number<<","<<temp_number<<")="<<ans[2]<<"\n";
        std::cout<<"Bezout coefficients are "<<ans[0]<<"   "<<ans[1]<<"\n";   
        break;
        }
        case MODULAR_INVERSE:
        std::cin>>current_number;
        std::cin>>temp_number;
        std::cout<<modular_inverse(current_number,temp_number)<<"\n";
        break;
        case sum_of_digits:
            std::cin>>current_number;
            std::cout<<sum_digits(current_number)<<"\n";
            break;
        case number_of_digits:
            std::cin>>current_number;
            std::cout<<current_number.number_of_digits()<<"\n";
            break;
        case random_big_prime:
            int size;
            std::cin>>size;
            if(size<0 || size>1000000){
                size=1;
            }
            current_number=random_prime_bigint(size);
            std::cout<<current_number<<"\n";
            break;
        case random_small_prime:
            current_number=primes[rand10e9(rng)%78498];
            std::cout<<current_number<<"\n";
            break;
        case nthprime:
            std::cin>>temp;
            if(temp<0){std::cout<<"Error. Enter positive integer\n";break;}
            if(temp>105097565){std::cout<<"Too high\n";goto again;}
            if(temp>78498 && primes_int_initialised==false){
                std::cout<<"Too high. Do you want to wait?"<<"\n";
                std::cin>>input;
                if(input=="yes"||input=="Y"||input=="y"||input=="YES"){
                    sieve_int();
                    primes_int_initialised=true;
                } else {
                    std::cout<<"Okay!"<<"\n";
                    goto again;
                }
            }
            current_number=primes[temp-1];
            std::cout<<current_number<<"\n";
            break;
        case factorl:
            std::cin>>temp;
            current_number=factorial(temp);
            std::cout<<current_number<<"\n";
            break;
        case primorial:{
        current_number=1;
        std::cin>>temp;
        if(temp<1 || temp>primes.size()-1){std::cout<<"Invalid. Maybe try init?"<<"\n";}
        else {for(int i=0;i<temp;i++){
            current_number*=primes[i];
        }
        std::cout<<current_number<<"\n";
        }
        break;
        }
        case bin:{
            int n;
            int k;
            std::cin>>n;
            std::cin>>k;
            std::cout<<binomial(n,k)<<"\n";
            break;
            }
        case fib:
            std::cin>>temp;
            current_number=fibonacci(temp);
            std::cout<<current_number<<"\n";
            break;
        case to_pow_bigint:
            std::cin>>current_number;
            std::cin>>temp;
            current_number=current_number.to_pow(temp);
            std::cout<<current_number<<"\n";
            break;
        case to_powmod_bigint:{
            bigint power;
            std::cin>>current_number;
            std::cin>>power;
            std::cin>>temp_number;
            current_number=pow_mod(current_number,power,temp_number);
            std::cout<<current_number<<"\n";
        break;
        }
        case count_primes_before:
            std::cin>>temp;
            if(temp<2){std::cout<<"0"<<"\n";}
            if(temp>1000000 && primes_int_initialised==false){
                std::cout<<"Too high. Do you want to wait?"<<"\n";
                std::cin>>input;
                if(input=="yes"||input=="Y"||input=="y"){
                    sieve_int();
                    primes_int_initialised=true;
                } else {
                    std::cout<<"Okay!"<<"\n";
                    goto again;
                }
            }
            if(temp>0){
                std::cout<<prime_count(temp)<<"\n";
            }
            break;
        case base_to_base:{
            std::string number;
            int old_base;
            int new_base;
            std::cin>>old_base;
            std::cin>>new_base;
            std::cin>>number;
            std::cout<<convert_base(old_base,new_base,number)<<"\n";
            break;
        }
        case bintodec:{
        std::string number;
        std::cin>>number;
        std::cout<<convert_base(2,10,number)<<"\n";
        break;
        }
        case bintohex:{
        std::string number;
        std::cin>>number;
        std::cout<<convert_base(2,16,number)<<"\n";
        break;
        }
        case dectohex:{
        std::string number;
        std::cin>>number;
        std::cout<<convert_base(10,16,number)<<"\n";
        break;
        }
        case dectobin:{
        std::string number;
        std::cin>>number;
        std::cout<<convert_base(10,2,number)<<"\n";
        break;
        }
        case hextodec:{
        std::string number;
        std::cin>>number;
        std::cout<<convert_base(16,10,number)<<"\n";
        break;
        }
        case hextobin:{
        std::string number;
        std::cin>>number;
        std::cout<<convert_base(16,2,number)<<"\n";
        break;
        }
        default:
            std::cout<<"no such command\n";
            break;
        }
    }

 }  
