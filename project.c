/* 
Since implementation has to be done for a 1024-bit integer we can calculate the number of bits required to
represent it as an integer
--> we have a 1024-bit integer which can at max represent an integer of value equivalent to 2^1024 - 1
--> if we take a n bit integer(reprented in the digits 0- 9) it can at max represent 10^n - 1
--> no equalling them and using logarithm we get n <= 309
    thus we can conlude that a 1024 bit binary integer can be easily represented in a 309 bit integer representation
*/

#include<stdio.h>
#include<stdlib.h>
typedef enum {POSITIVE,NEGATIVE} sign_Status;
typedef enum {FALSE,TRUE} bool;

typedef struct node_tag{
    short int data;
    struct node_tag* next;
    struct node_tag* prev;
}Node;

typedef struct int_tag{
    Node* head;
    Node* tail;
    int size;
    sign_Status sign;
}BigInt;

Node* createNode(short int val){
    Node* ptr = (Node *)malloc(sizeof(Node));
    ptr->data = val;
    ptr->next = NULL;
    ptr->prev = NULL;
    return ptr;
}

void initializeBigInt(BigInt *B){
    B->head = NULL;
    B->tail = NULL;
    B->sign = POSITIVE;
    B->size = 0;
}

int length(char s[]){
    int i = 0;
    while(s[i] != '\0'){
        i++;
    }
    return i;
}

void printDigits(Node* head){
    if(head == NULL){
        return;
    }
    printDigits(head->next);
    printf("%hd",head->data);
}

void displayNum(BigInt* num){
    if(num->head == NULL){
        printf("NULL VALUE\n");
    }
    else{
        if(num->sign == NEGATIVE){
            printf("-");
        }
        Node* ptr = num->head;
        printDigits(ptr);
    }
}

bool isFormatValid(char s[]){
    bool flag = TRUE;
    int i = 0;
    if(s[0] == ' ' || s[0] == '+' || s[0] == '-'){
        i++;
    }
    else{
        flag = FALSE;
    }
    int n = length(s) - 1;
    while(i < n && flag){
        int t = s[i] - '0';
        if(!(t <= 9 && t>= 0)){
            flag = FALSE;
        }
        i++;
    }
    return flag;
}

void AddatTail(Node** head,Node** tail,short int val){
    Node* new = createNode(val);
    if(*head == NULL){
        *head = new;
        *tail = *head;
    }
    else{
        (*tail)->next = new;
        new->prev = (*tail);
        (*tail) = (*tail)->next;
    }
}

void readNum(BigInt* num, char s[]){
    int n = length(s) - 1;
    while(n > 0 && num->size < 309){
        AddatTail(&(num->head),&(num->tail),(s[n] - '0'));
        n--;
        (num->size)++;
    }
    if(s[0] == '-'){
        num->sign = NEGATIVE;
    }
}

bool isBigIntZero(BigInt* num){
    bool ans;
    if(num->size == 1 && num->head->data == 0)
        ans = TRUE;
    
    else
        ans = FALSE;
    
    return ans;
}

int compareNum(BigInt* num1,BigInt* num2){ // returns -1 if num1 < num2 &    0 if num1 == num2 &        1 if num1 > num2
    int ret_val;
    if(num1->sign == POSITIVE && num2->sign == POSITIVE){
        if(num1->size > num2->size){
            ret_val = 1;
        }
        else if(num1->size < num2->size){
            ret_val = -1;
        }
        else{
            Node* ptr1 = num1->tail;
            Node* ptr2 = num2->tail;
            while(ptr1 && ptr2 && (ptr1->data == ptr2->data)){
                ptr1 = ptr1->prev;
                ptr2 = ptr2->prev;
            }
            if(ptr1 == NULL && ptr2 == NULL){
                ret_val = 0;
            }
            else{
                if(ptr1->data > ptr2->data){
                    ret_val = 1;
                }
                else{
                    ret_val = -1;
                }
            }
        }
    }
    else if(num1->sign == POSITIVE && num2->sign == NEGATIVE){
        ret_val = 1;
    }
    else if(num1->sign == NEGATIVE && num2->sign == POSITIVE){
        ret_val = -1;
    }
    else if(num1->sign == NEGATIVE && num2->sign == NEGATIVE){
        if(num1->size > num2->size){
            ret_val = -1;
        }
        else if(num1->size < num2->size){
            ret_val = 1;
        }
        else{
            Node* ptr1 = num1->tail;
            Node* ptr2 = num2->tail;
            while(ptr1 && ptr2 && (ptr1->data == ptr2->data)){
                ptr1 = ptr1->prev;
                ptr2 = ptr2->prev;
            }
            if(ptr1 == NULL && ptr2 == NULL){
                ret_val = 0;
            }
            else{
                if(ptr1->data > ptr2->data){
                    ret_val = -1;
                }
                else{
                    ret_val = 1;
                }
            }
        }
    }
    return ret_val;
}

int max(int a,int b){
    int ans = (a > b) ? a : b;
    return ans;
}

BigInt addNumbers(BigInt* num1,BigInt* num2){
    BigInt ans;
    initializeBigInt(&ans);

    if(num1->head == NULL || num2->head == NULL){
        printf("Add operation cannot be performed since ");
        printf("atleast One of the numbers you entered is invalid , please enter numbers correctly ");
        printf("returning NULL values as answer \n\n");
        return ans;
    }

    int flag = 1;
    int size = max(num1->size,num2->size);
    if(num1->sign == num2->sign){
        Node* ptr1 = num1->head;
        Node* ptr2 = num2->head;
        short int carry = 0;
        while(ptr1 || ptr2 &&(flag)){
            short int a = (ptr1 != NULL) ? ptr1->data : 0;
            short int b = (ptr2 != NULL) ? ptr2->data : 0;
            short int sum = a + b + carry;
            carry = sum / 10;
            sum = sum % 10;
            AddatTail(&(ans.head),&(ans.tail),sum);
            if(ptr1)
                ptr1 = ptr1->next;
            if(ptr2)
                ptr2 = ptr2->next;
            if(ptr1 == NULL && ptr2 == NULL){
                if(carry && size < 309){
                    AddatTail(&(ans.head),&(ans.tail),carry);
                    size++;
                }
                else if(carry && size >= 309){
                    flag = 0;
                }
            }
        }
        ans.size = size;
        if(((num1->sign == NEGATIVE) && (num2->sign == NEGATIVE)))
            ans.sign = NEGATIVE;
    }

    else if((num1->sign == NEGATIVE) ^ (num2->sign == NEGATIVE)){
        BigInt tempnum1;
        BigInt tempnum2;
        if(num1->sign == NEGATIVE){
            num1->sign = POSITIVE;
            int compare = compareNum(num1,num2);
            if(compare == -1){
                tempnum1 = *num2;
                tempnum2 = *num1;
                ans.sign = POSITIVE;
                num1->sign = NEGATIVE;
            }
            else if(compare == 1){
                tempnum1 = *num1;
                tempnum2 = *num2;
                ans.sign = NEGATIVE;
                num1->sign = NEGATIVE;
            }
            else if(compare == 0){
                Node* temp = createNode(0);
                ans.head = ans.tail = temp;
                (ans.size)++;
                num1->sign = NEGATIVE;
                return ans;
            }
        }
        if(num2->sign == NEGATIVE){
            num2->sign = POSITIVE;
            int compare = compareNum(num1,num2);
            if(compare == -1){
                tempnum1 = *num2;
                tempnum2 = *num1;
                num2->sign = NEGATIVE;
                ans.sign = NEGATIVE;
            }
            else if(compare == 1){
                tempnum1 = *num1;
                tempnum2 = *num2;
                num2->sign = NEGATIVE;
                ans.sign = POSITIVE;
            }
            else if(compare == 0){
                Node* temp = createNode(0);
                ans.head = ans.tail = temp;
                (ans.size)++;
                num2->sign = NEGATIVE;
                return ans;
            }
        }
        Node* ptr1 = tempnum1.head;
        Node* ptr2 = tempnum2.head;
        short int borrow = 0;
        while(ptr1 || ptr2){
            short int x = (ptr1 != NULL) ? ptr1->data : 0;
            short int y = (ptr2 != NULL) ? ptr2->data : 0;
            short int s  = x - borrow;
            borrow = 0;
            if(s < y){
                s = s + 10;
                borrow++;
            }
            short int diff = s - y;
            AddatTail(&(ans.head),&(ans.tail),diff);
            if(ptr1)
                ptr1 = ptr1->next;
            if(ptr2)
                ptr2 = ptr2->next;
        }
        ans.size = size;
    }
    
    Node* temp = ans.tail;
    //identifying leading zeroes if any
    while(temp && temp->data == 0){
        temp = temp->prev;
    }
    //resetting the values
    if(temp){
        ans.tail = temp;
        temp = temp->next;
        ans.tail->next = NULL;
    }
    else{
        temp = ans.head->next;
        ans.head->next = NULL;
    }
    while(temp){
        Node* x = temp;
        temp = temp->next;
        free(x);
    }
    if(!flag){
        printf("\nBit Overflow (or) UnderFlow\n");
        printf("The returned ans is inaccurate\n");
    }
    return ans;
}

BigInt subtractNumbers(BigInt* num1,BigInt* num2){ // returns a number whose result is NUM1 - NUM2
    BigInt ans;
    initializeBigInt(&ans);

    if(num1->head == NULL || num2->head == NULL){
        printf("Subtraction operation cannot be performed since ");
        printf("atleast One of the numbers you entered is invalid , please enter numbers correctly ");
        printf("returning NULL values as answer \n\n");
        return ans;
    }

    num2->sign = !(num2->sign);
    ans = addNumbers(num1,num2);
    num2->sign = !(num2->sign);
    return ans;
}

BigInt multiplyNumbers(BigInt* num1, BigInt* num2) {
    BigInt ans;
    initializeBigInt(&ans);
    int flag = 1;
    if(num1->head == NULL || num2->head == NULL){
        printf("Multiplication operation cannot be performed since ");
        printf("atleast One of the numbers you entered is invalid , please enter numbers correctly ");
        printf("returning NULL values as answer \n\n");
        return ans;
    }

    if(isBigIntZero(num1) || isBigIntZero(num2)){
        Node* temp = createNode(0);
        ans.head = ans.tail = temp;
        ans.size++;
        return ans;
    }

    Node* arr[309];
    int idx = 0;
    for (int i = 0; i < 309; i++) {
        arr[i] = NULL;
    }
    int size = 1;
    Node* ptr1 = num1->head;
    while (ptr1) {
        Node* ptr2 = num2->head;
        Node* temphead = NULL;
        Node* temptail = temphead;
        short int carry = 0;
        short int x = ptr1->data;

        while (ptr2) {
            short int mul = (x * ptr2->data) + carry;
            carry = mul / 10;
            mul = mul % 10;
            AddatTail(&temphead, &temptail, mul);
            ptr2 = ptr2->next;
        }

        if (carry) {
            AddatTail(&temphead, &temptail, carry);
        }

        arr[idx++] = temphead;
        ptr1 = ptr1->next;
    }

    Node* maintail = arr[0];

    while (maintail->next) {
        size++;
        maintail = maintail->next;
    }

    int threshold = 1;
    Node* head = arr[0];
    for (int i = 1; i < idx; i++) {
        Node* mainhead = arr[0];
        int t = threshold;
        while(t > 0){
            mainhead = mainhead->next;
            t--;
        }
        short int carry = 0;
        Node* temp = arr[i];
        while (mainhead && temp && size <= 309) {
            short int sum = (mainhead->data) + (temp->data) + carry;
            carry = sum / 10;
            sum = sum % 10;
            mainhead->data = sum;
            mainhead = mainhead->next;
            temp = temp->next;
        }
        while (temp && size < 309) {
            short int sum = (temp->data) + carry;
            carry = sum / 10;
            sum = sum % 10;
            AddatTail(&(head),&(maintail),sum);
            temp = temp->next;
            size++;
            if(size >= 309){
                flag = 0;
            }
        }
        if(carry  && size < 309){
            AddatTail(&(head),&(maintail),carry);
            size++;
        }
        else if(carry && size >= 309){
            flag = 0;
        }
        threshold++;
    }

    ans.head = arr[0];
    ans.tail = maintail;
    ans.size = size;

    for(int i = 1;i < idx;i++){ //freeing out all the steps that were stored in the heap
        Node* temp = arr[i];
        while(temp){
            Node* f = temp;
            temp = temp->next;
            free(f);
        }
        arr[i] = NULL;
    }

    if(num1->sign == num2->sign){
        ans.sign = POSITIVE;
    }
    else{
        ans.sign = NEGATIVE;
    }
    if(!flag){
        printf("\n\n\n");
        printf("In the Multiplication function\n");
        printf("\nBit Overflow (or) UnderFlow\n");
        printf("The returned ans is inaccurate\n");
    }
    Node* temp = ans.tail;
    //identifying leading zeroes if any
    while(temp && temp->data == 0){
        temp = temp->prev;
    }
    //resetting the values
    if(temp){
        ans.tail = temp;
        temp = temp->next;
        ans.tail->next = NULL;
    }
    else{
        temp = ans.head->next;
        ans.head->next = NULL;
    }
    while(temp){
        Node* x = temp;
        temp = temp->next;
        free(x);
    }
    return ans;
}

int main()
{
    BigInt num1;
    initializeBigInt(&num1);
    BigInt num2;
    initializeBigInt(&num2);
    system("cls");
    printf("\n\nIf the number is positive then enter it in the format of + sign (or) blank space followed by the number  ");
    printf("Else if the number is negative then enter it in the format of - sign followed by the number  \n\n");
    char s[309]; // due to above logic fixed size to 309
    
    char opstatus = 'Y';
    printf("\nEnter The bigint : \n");
    gets(s);

    while(!isFormatValid(s)){
        printf("Please enter number in specified format\n");
        printf("Reenter a new BigInt : \n");
        fflush(stdin);
        gets(s);
    }
    readNum(&num1,s);

    printf("\nenter The bigint : \n");
    gets(s);
    while(!isFormatValid(s)){
        printf("Please enter number in specified format\n");
        printf("Reenter a new BigInt : \n");
        fflush(stdin);
        gets(s);
    }
    readNum(&num2,s);
    BigInt num3 = addNumbers(&num1,&num2);
    printf("\n\n\n");
    printf("\nsum = \n");
    displayNum(&num3);

    BigInt num4 = subtractNumbers(&num1,&num2);
    printf("\n\n\n");
    printf("\ndifference = \n");
    displayNum(&num4);

    BigInt Num3 = multiplyNumbers(&num1,&num2);
    printf("\n\n\n");
    printf("\nproduct of numbers = \n");
    displayNum(&Num3);

    return 0;
}