/* 
Since implementation has to be done for a 1024-bit integer we can calculate the number of bits required to
represent it as an integer and create a array of char type of that size
--> n bit integer can represent upto (2^n-1) -1
--> we have a 1024-bit integer which can at max represent an integer of value equivalent to 2^1023 - 1
--> if we take a n digit integer(reprented in the digits 0- 9) it can at max represent (10^n-1) - 1
--> now equalling them and using logarithm we get n <= 309
--> thus we can conlude that a 1024 bit binary integer can be easily represented in a 309 bit integer representation
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h> //for unsigned integers of 8bits(1byte)

#define BIGINT_SIZE 311 //309 digits + 1 for sign + 1 for \0(null termiantor)
#define MAX_DIGITS 309 //maximum digits is 309 digits as explained above
//as we are using 8 bit unsigned integers to store digits
#define BASE 10
#define KARATSUBA_THRESHOLD 32 // threshold for Karatsuba multiplication, can be adjusted based on performance needs

typedef enum {FALSE, TRUE} bool;
typedef enum {POSITIVE, NEGATIVE} numSign;
typedef enum {SMALL = -1, EQUAL, LARGE} compareStatus;

typedef struct BigInt {
    uint8_t* digit_array;// as my numbers are from 0 - 9, so they can be stored unsigned 8bit integers (rang : 0 - 255)
    numSign sign; //negative = 1, positive = 0
    short size;//as size <= 311 so it can be stored in short (range : -32768 to 32767)
}BigInt;

short max (short a, short b) {
    return (a > b) ? a : b;
}

short string_length (char *str) {
    short length = 0;

    //first digit denotes sign of the integer, so i starts from 1;
    if (str != NULL) {
        short i = 1;
        while (str[i] != '\0') {
            length += 1;
            i += 1;
        }
    }

    return length;
}

bool isValidInput (char* str) { // to ensure given input contains only specified format
    bool ans = TRUE;
    short i = 0, length;
    length = string_length(str);

    if (str[i] == ' ' || str[i] == '+' || str[i] == '-') {
        i = i + 1;
        while (i != length && ans) {
            if (str[i] < 48 || str[i] > 57) { // 0 - ascii value - 48 and 9 ascii value - 57 
                ans = FALSE;
            }
            i += 1;
        }
    } else {
        ans = FALSE;
    }

    return ans;
}

//This function is checked only when the input string is in valid form
numSign signSpecifier (char *str) {
    numSign sign;

    if (str[0] == '+' || str[0] == ' ') {
        sign = POSITIVE;
    } else if (str[0] == '-') {
        sign = NEGATIVE;
    }

    return sign;
}

void initializeBigInt (BigInt* b, short size, numSign sign) {
    b -> digit_array = (uint8_t*) calloc(size , sizeof(uint8_t));

    if(b -> digit_array != NULL) { //safe side so that we dont get into segmentation fault by accesing null
        b -> sign = sign;
        b -> size = size;
    } else {
        free(b -> digit_array);
        b -> digit_array = NULL;
        printf("memory allocation failed.\n");
        exit(EXIT_FAILURE); //stops execution and exits status is failure(1)
    }
}

BigInt readNumFromString (char *str) {
    BigInt b;
    numSign sign = signSpecifier(str);
    short length = string_length(str);
    initializeBigInt(&b, length, sign);
    short i = 1;

    while (str[i] != '\0') {
        uint8_t digit = str[i] - '0';
        b.digit_array[i-1] = digit;
        i += 1;
    }
    return b;
}

//sign not considered
compareStatus compareMagnitude (BigInt b1, BigInt b2) {
    compareStatus cs;

    if (b1.size > b2.size) {
        cs = LARGE;
    } else if (b1.size < b2.size) {
        cs = SMALL;
    } else {
        short size = b1.size;
        short i = 0;
        while (i < size && b1.digit_array[i] == b2.digit_array[i]) {
            i = i + 1;
        }
        if (i == size) {
            cs = EQUAL;
        } else if (b1.digit_array[i] > b2.digit_array[i]) {
            cs = LARGE;
        } else if (b1.digit_array[i] < b2.digit_array[i]) {
            cs = SMALL;
        }
    }

    return cs;
}

//sign considered
compareStatus compareNumbers (BigInt b1, BigInt b2) {
    compareStatus cs;
    
    //positive is always larger than negative
    if (b1.sign == POSITIVE && b2.sign == NEGATIVE) {
        cs = LARGE;
    } else if (b1.sign == NEGATIVE && b2.sign == POSITIVE) {
        cs = SMALL;
    }

    if (b1.sign == POSITIVE) {
        cs =  compareMagnitude(b1, b2);
    } else {
        compareStatus result = compareMagnitude(b1, b2);
        if (result == LARGE) {
            cs = SMALL;
        } else if (result == SMALL) {
            cs = LARGE;
        } else {
            cs = EQUAL;
        }
    }

    return cs;
}

bool isBigIntZero (BigInt b) {
    short i = 0;
    bool ans;

    while (i < b.size && b.digit_array[i] == 0) {
        i++;
    }

    if(i == b.size) {
        ans = TRUE;
    } else {
        ans = FALSE;
    }

    return ans;
}

short size_diff (BigInt b1, BigInt b2) {
    return abs(b1.size - b2.size);
}

//function assumes that b1 > b2
BigInt subraction (BigInt b1, BigInt b2, short size, numSign sign) {
    if (size > MAX_DIGITS) {
        printf("Overflow Warning: Subtraction result size %d exceeds max allowed %d digits.\n", size, MAX_DIGITS);
    }
    BigInt b;
    initializeBigInt(&b, size, sign);
    short i = b1.size - 1;
    short j = b2.size - 1;
    short sizeDiff = size_diff(b1, b2);
    uint8_t borrow = 0;

    while (i >= 0 && j >= 0) {
        int8_t digitDiff = b1.digit_array[i] - b2.digit_array[j] - borrow;
        if (digitDiff < 0) {
            borrow = 1;
            digitDiff += 10;
        } else {
            borrow = 0;
        }
        b.digit_array[i] = digitDiff; 
        i -= 1;
        j -= 1;
    }

    while (i >= 0) {
        int8_t digitDiff = b1.digit_array[i] - borrow;
        if (digitDiff < 0) {
            borrow = 1;
            digitDiff += 10;
        } else {
            borrow = 0;
        }
        b.digit_array[i] = digitDiff; 
        i -= 1;
    }
    //as arg1 > arg2 so we dont check for j as j will always approach zero first

    return b;
}

BigInt addTwoNumbers (BigInt b1, BigInt b2) {
    BigInt b;
    numSign sign;
    compareStatus cs1, cs;

    cs1 = compareNumbers(b1, b2);
    cs = compareMagnitude(b1, b2);

    if (b1.sign == b2.sign) {
        short sizeDiff = size_diff(b1, b2);
        short length = max(b1.size, b2.size) + 1; // extra 1 block for carry. if carry exists actually it overflows so to prevent overflow extra one block
        if (length > MAX_DIGITS) {
            printf("Overflow Warning: Addition result exceeds %d digits.\n", MAX_DIGITS);
        }
        sign = b1.sign;
        initializeBigInt(&b, length, sign);
        short size;
        size = max(b1.size, b1.size);
        short i = b1.size - 1;
        uint8_t carry = 0;
        short j = b2.size - 1;

        while (i >= 0 && j >= 0) {
            uint8_t digitSum = b1.digit_array[i] + b2.digit_array[j] + carry;
            uint8_t digit = digitSum % 10;
            short k = (b1.size > b2.size) ? i : j;
            b.digit_array[k + 1] = digit;
            carry = digitSum / 10;
            i -= 1;
            j -= 1;
        }
        while (i >= 0) {
            uint8_t digitSum = b1.digit_array[i] + carry;
            uint8_t digit = digitSum % 10;
            b.digit_array[i + 1] = digit;
            carry = digitSum / 10;
            i -= 1;
        }
        while (j >= 0) {
            uint8_t digitSum = b2.digit_array[j] + carry;
            uint8_t digit = digitSum % 10;
            b.digit_array[j + 1] = digit;
            carry = digitSum / 10;
            j -= 1;
        }

        short k = (b1.size > b2.size) ? i : j;
        if (carry) {
            b.digit_array[k + 1] = 1;
        } else {
            b.digit_array[k + 1] = 0;
        }
    } else {
        short length = max(b1.size, b2.size); //the max size could not exceed maximum of two numbers in subraction

        if (b1.sign == POSITIVE && b2.sign == NEGATIVE) {
            if (cs == LARGE || cs == EQUAL) {
                sign = POSITIVE;
                b = subraction(b1, b2, length, sign);
            } else {
                sign = NEGATIVE;
                b = subraction(b2, b1, length, sign);
            }
        } else {
            if (cs == LARGE || cs == EQUAL) {
                sign = NEGATIVE;
                b = subraction(b1, b2, length, sign);
            } else {
                sign = POSITIVE;
                b = subraction(b2, b1, length, sign);
            }
        }
    }

    return b;
}

BigInt subtracTwotBigInts (BigInt b1, BigInt b2) {
    BigInt b;
    short length = max(b1.size, b2.size); 
    compareStatus cs = compareMagnitude(b1, b2);
    numSign resultSign;

    //just use above add function with taking care of signs
    if (b1.sign == POSITIVE && b2.sign == POSITIVE) {
        //b1 - b2 = b1 - b2
        if (cs == LARGE || cs == EQUAL) {
            resultSign = POSITIVE;
            b = subraction(b1, b2, length, resultSign);
        } else {
            resultSign = NEGATIVE;
            b = subraction(b2, b1, length, resultSign);
        }
    } else if (b1.sign == POSITIVE && b2.sign == NEGATIVE) {
        // b1 - (-b2) = b1 + b2
        resultSign = POSITIVE;
        b2.sign = POSITIVE;
        b1.sign = POSITIVE;
        b = addTwoNumbers(b1, b2);
        b.sign = resultSign;
    } else if (b1.sign == NEGATIVE && b2.sign == POSITIVE) {
        // (-b1) - b2 = -(b1 + b2)
        resultSign = NEGATIVE;
        b2.sign = POSITIVE;
        b1.sign = POSITIVE;
        b = addTwoNumbers(b1, b2);
        b.sign = resultSign;
    } else {
        // (-b1) - (-b2) = b2 - b1
        if (cs == LARGE || cs == EQUAL) {
            resultSign = NEGATIVE;
            b = subraction(b1, b2, length, resultSign);
        } else {
            resultSign = POSITIVE;
            b = subraction(b2, b1, length, resultSign);
        }
    }

    return b;
}

//I store the whole result in a bigint structure in which array could have maximum size of 618(309 + 309)
BigInt multiplyTwoBigInt (BigInt b1, BigInt b2) {
    BigInt b;
    numSign sign = b1.sign ^ b2.sign;
    short size = b1.size + b2.size;
    if (size > MAX_DIGITS) {
        printf("Overflow Warning: result size exceeds MAX_DIGITS\n");
    }
    initializeBigInt(&b, size, sign);
    b.sign = POSITIVE;
    short size1 = b1.size;
    short size2 = b2.size;
    short extraSpacing = 0;

    for (short i = size2 - 1; i >= 0; i--) {
        BigInt pb;
        //array with extraspacing, the extra spacing has zeroes because as we shift left by 1 and add 0 at right and continues this method
        if (size1 + 1 + extraSpacing > MAX_DIGITS) {
            printf("Overflow Warning: partial product size exceeds MAX_DIGITS\n");
        }
        initializeBigInt(&pb, size1 + 1 + extraSpacing, POSITIVE);
        int8_t carry = 0;
        short j;
        short ind = pb.size - 1 - extraSpacing;
        for (j = size1 - 1; j >= 0; j--)  {
            int8_t digitMulti = (b2.digit_array[i] * b1.digit_array[j]) + carry; 
            pb.digit_array[ind] = digitMulti % 10;
            carry = digitMulti / 10;
            ind--;
            if (ind < 0 && (j > 0 || carry > 0)) {
                printf("Index overflow in multiplication loop\n");
            }
        }
        if (carry != 0) {
            if (ind < 0) {
                printf("Index overflow while placing carry\n");
            }
            pb.digit_array[ind] = carry;
            ind--;
        }
        extraSpacing += 1;
        b = addTwoNumbers(b, pb);
        free (pb.digit_array);
    }
    b.sign = sign;

    return b;
}

void freeBigInt(BigInt *b) {
    if (b == NULL) return;    
    if (b->digit_array != NULL) {
        free(b->digit_array);
        b->digit_array = NULL;  
    }
    b->size = 0;
    b->sign = POSITIVE;
}

BigInt shiftLeft(BigInt b, int n) {
    BigInt res;
    initializeBigInt(&res, b.size + n, b.sign);
    memset(res.digit_array + res.size - n, 0, n);

    for (int i = 0; i < b.size; i++) {
        res.digit_array[i] = b.digit_array[i];
    }
    return res;
}

// Karatsuba multiplication recursive
BigInt karatsubaMultiply(BigInt x, BigInt y) {
    int n = x.size > y.size ? x.size : y.size;

    // Base case: use naive multiplication for small numbers
    if (n <= KARATSUBA_THRESHOLD) {
        return multiplyTwoBigInt(x, y);
    }

    int half = n / 2;

    // Split x into high and low parts
    BigInt x_high, x_low;
    initializeBigInt(&x_high, x.size - half > 0 ? x.size - half : 0, POSITIVE);
    initializeBigInt(&x_low, half, POSITIVE);

    memcpy(x_high.digit_array, x.digit_array, (x.size - half) * sizeof(uint8_t));
    memcpy(x_low.digit_array, x.digit_array + x.size - half, half * sizeof(uint8_t));

    // Split y into high and low parts
    BigInt y_high, y_low;
    initializeBigInt(&y_high, y.size - half > 0 ? y.size - half : 0, POSITIVE);
    initializeBigInt(&y_low, half, POSITIVE);

    memcpy(y_high.digit_array, y.digit_array, (y.size - half) * sizeof(uint8_t));
    memcpy(y_low.digit_array, y.digit_array + y.size - half, half * sizeof(uint8_t));

    // Compute three products recursively
    BigInt z0 = karatsubaMultiply(x_low, y_low);
    BigInt z2 = karatsubaMultiply(x_high, y_high);

    BigInt x_sum = addTwoNumbers(x_low, x_high);
    BigInt y_sum = addTwoNumbers(y_low, y_high);
    BigInt z1 = karatsubaMultiply(x_sum, y_sum);

    // z1 = z1 - z2 - z0
    BigInt temp = subtracTwotBigInts(z1, z2);
    BigInt z1_final = subtracTwotBigInts(temp, z0);

    // Combine results:
    // result = z2 * BASE^{2*half} + z1 * BASE^{half} + z0

    BigInt z2_shift = shiftLeft(z2, 2 * half);
    BigInt z1_shift = shiftLeft(z1_final, half);

    BigInt temp_sum = addTwoNumbers(z2_shift, z1_shift);
    BigInt result = addTwoNumbers(temp_sum, z0);

    // Set sign
    result.sign = x.sign ^ y.sign;

    // Free temporaries
    freeBigInt(&x_high);
    freeBigInt(&x_low);
    freeBigInt(&y_high);
    freeBigInt(&y_low);
    freeBigInt(&z0);
    freeBigInt(&z1);
    freeBigInt(&z2);
    freeBigInt(&x_sum);
    freeBigInt(&y_sum);
    freeBigInt(&temp);
    freeBigInt(&z1_final);
    freeBigInt(&z2_shift);
    freeBigInt(&z1_shift);
    freeBigInt(&temp_sum);

    return result;
}

BigInt divideBigInt(BigInt dividend, BigInt divisor) {
    if (isBigIntZero(divisor)) {
        printf("Error: Division by zero\n");
        BigInt result;
        initializeBigInt(&result, 1, POSITIVE);
        result.digit_array[0] = 0;
        return result;
    }
    BigInt quotient;
    initializeBigInt(&quotient, dividend.size, POSITIVE);
    
    if (compareMagnitude(dividend, divisor) == SMALL) {
        quotient.digit_array[0] = 0;
        quotient.size = 1;
        return quotient;
    }

    BigInt current;
    initializeBigInt(&current, dividend.size, POSITIVE);
    short current_size = 0;
    
    short quotient_pos = 0;
    bool started = FALSE;
    
    for (short i = 0; i < dividend.size; i++) {
        if (current_size > 0 || dividend.digit_array[i] > 0) {
            current.digit_array[current_size++] = dividend.digit_array[i];
            current.size = current_size;
        }
        
        if (current_size > 0) {
            uint8_t count = 0;
            while (compareMagnitude(current, divisor) != SMALL) {
                BigInt temp = subraction(current, divisor, current.size, POSITIVE);
                free(current.digit_array);
                current = temp;
                count++;

                short j = 0;
                while (j < current.size - 1 && current.digit_array[j] == 0) {
                    j++;
                }
                if (j > 0) {
                    memmove(current.digit_array, current.digit_array + j, current.size - j);
                    current_size = current.size - j;
                    current.size = current_size;
                }
            }
            
            if (count > 0 || started) {
                quotient.digit_array[quotient_pos++] = count;
                started = TRUE;
            }
        }
    }

    if (quotient_pos == 0) {
        quotient.size = 1;
        quotient.digit_array[0] = 0;
    } else {
        quotient.size = quotient_pos;
    }

    quotient.sign = (dividend.sign == divisor.sign) ? POSITIVE : NEGATIVE;
    free(current.digit_array);
    
    return quotient;
}

void printDigits(BigInt b) {
    short size = b.size;
    short i = 0;

    //to make sure starting zeroes are not printed
    while (i < size - 1 && b.digit_array[i] == 0) {
        i = i + 1;
    }

    if (isBigIntZero(b)) {
        printf("0\n");
        return;
    }

    char sign = (b.sign == POSITIVE) ? '+' : '-';
    printf("%c", sign);

    for (; i < size; i++) {
        printf("%u", b.digit_array[i]);
    }

    printf("\n");
}

BigInt copyBigInt (BigInt b) {
    BigInt b1;
    initializeBigInt(&b1, b.size, b.sign);
    for (int i = 0; i < b.size; i++) {
        b1.digit_array[i] = b.digit_array[i];
    }
    return b1;
}

BigInt input_string_1 () {
    printf("enter first number : ");
    char* input_string1 = (char*) malloc(sizeof(char) * BIGINT_SIZE);
    if (input_string1 == NULL) {
        free(input_string1);
        fprintf(stderr, "Memory allocation failed is1\n");
        exit(EXIT_FAILURE);
    }

    bool validInput = FALSE;
    while (!validInput) {
        scanf("%310s", input_string1);
        // Check if input was too long
        int c;
        if ((c = getchar()) != '\n' && c != EOF) {
            // Clear input buffer
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Input too long - maximum 309 digits allowed.\n");
            printf("Please enter a shorter number: ");
            continue;  // Go back to start of loop for new input
        }
        // Check if input format is valid
        if (!isValidInput(input_string1)) {
            printf("Please enter valid input: ");
            continue;  // Go back to start of loop for new input
        }
        validInput = TRUE;  // If we get here, input is valid
    }
     
    BigInt b1;
    printf("first number : ");
    b1 = readNumFromString(input_string1);
    printDigits(b1);
    return b1;
}

BigInt input_string_2 () {
    printf("enter second number : ");
    char* input_string2 = (char*) malloc(sizeof(char) * BIGINT_SIZE);
    if (input_string2 == NULL) {
        free(input_string2);
        fprintf(stderr, "Memory allocation failed is2\n");
        exit(EXIT_FAILURE);
    }

    bool validInput = FALSE;
    while (!validInput) {
        scanf("%310s", input_string2);    
        // Check if input was too long
        int c;
        if ((c = getchar()) != '\n' && c != EOF) {
            // Clear input buffer
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Input too long - maximum 309 digits allowed.\n");
            printf("Please enter a shorter number: ");
            continue;
        }

        // Check if input format is valid
        if (!isValidInput(input_string2)) {
            printf("Please enter valid input: ");
            continue;
        }

        validInput = TRUE;
    }

    BigInt b2;
    printf("second number : ");
    b2 = readNumFromString(input_string2);
    printDigits(b2);
    return b2;
}

int main() {
    BigInt b1;
    BigInt b2;
    system("cls"); //clears terminal every times it runs
    printf("Operations :\n");
    printf("1 : enter 1 for Addition\n");
    printf("2 : enter 2 for Subraction\n");
    printf("3 : enter 3 for Multiplication\n");
    printf("4 : enter 4 for Division\n");
    printf("5 : enter 5 to exit\n");

    int op;
    printf("enter your option : ");
    scanf("%d", &op);
    BigInt b;

    //this does not work as calculator but this just do some operations with above two bigints and make inteface for it
    while (op != 5 && op < 5 && op > 0) {
        bool flag = TRUE;

        switch (op) {

            case 1 : {
                printf("if the number is positive enter the number with sign(+) or you can leave blank space(i.e press space bar once and enter the number) and if the number is negative enter the number with sign(-)\n");
                b1 = input_string_1();
                b2 = input_string_2();

                b = addTwoNumbers(b1, b2);
                printf("sum of two numbers is : ");
                printDigits(b);
                freeBigInt(&b); 
                break;
            }

            case 2 : {
                printf("if the number is positive enter the number with sign(+) or you can leave blank space(i.e press space bar once and enter the number) and if the number is negative enter the number with sign(-)\n");
                b1 = input_string_1();
                b2 = input_string_2();

                b = subtracTwotBigInts(b1, b2);
                printf("difference of two numbers is : ");
                printDigits(b);
                freeBigInt(&b);
                break;
            }

            case 3 : {
                printf("if the number is positive enter the number with sign(+) or you can leave blank space(i.e press space bar once and enter the number) and if the number is negative enter the number with sign(-)\n");
                b1 = input_string_1();
                b2 = input_string_2();
    
                BigInt b_karatsuba = karatsubaMultiply(b1, b2);
                printf("product of two numbers is : ");
                printDigits(b_karatsuba);
                freeBigInt(&b_karatsuba);
                break;
            }

            case 4 : {
                printf("if the number is positive enter the number with sign(+) or you can leave blank space(i.e press space bar once and enter the number) and if the number is negative enter the number with sign(-)\n");
                b1 = input_string_1();
                b2 = input_string_2();
                b = divideBigInt(b1, b2);
                printf("division of two numbers is : ");
                printDigits(b);
                break;
            }

            case 5 : {
                flag = FALSE;
                break;
            }

            default : {
                flag = FALSE;
                break;
            }
        }

        if (flag) {
            printf("Operations :\n");
            printf("1 : enter 1 for Addition\n");
            printf("2 : enter 2 for Subraction\n");
            printf("3 : enter 3 for Multiplication\n");
            printf("4 : enter 4 for divison.\n");
            printf("5 : enter 5 to exit\n");
            printf("enter your option : ");
            scanf("%d", &op);
        }
    }

    (op == 5) ? printf("exited\n") : printf("you entered wrong number. exited \n");

    freeBigInt(&b1);
    freeBigInt(&b2);

    return 0;
}
