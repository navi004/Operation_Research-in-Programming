#include <stdio.h>
#include <stdlib.h>

#define MAX 20

void printTable(float table[MAX][MAX], int rows, int cols, int n, int m, float CB[], int B[]) {
    printf("\nSimplex Table:\n");
    
    // Print the column headers
    printf("%7s", " ");
    printf("%7s", "CB");  
    printf("%7s", "B");  
    for (int j = 0; j < n; j++) {
        printf("%7s%d", "x", j + 1);  
    }
    for (int j = 0; j < m; j++) {
        printf("%7s%d", "S", j + 1);  
    }
    printf("%7s", "RHS");  
    printf("\n");

    // Print the rows of the tableau
    for (int i = 0; i < rows; i++) {
        if (i == 0) {
            printf("%7s", "Z"); 
            printf("%7s", " ");  
            printf("%7s", " ");  
        } else {
            printf("%7s%d", "C", i);  
            printf("%7.2f", CB[i - 1]);   

            // Print the basic variable (decision variable or slack variable)
            if (B[i - 1] <= n) {
                printf("%7s%d", "x", B[i - 1]); 
            } else {
                printf("%7s%d", "S", B[i - 1] - n);  
            }
        }

        // Print the elements of the tableau
        for (int j = 0; j < cols; j++) {
            printf("%7.2f ", table[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int selectPivotCol(float table[MAX][MAX], int rows, int cols, float CB[]) {
    float Zj[MAX];
    float Zj_minus_Cj[MAX];
    int pivotCol = -1;
    float minValue = 0.00;

    for (int i = 0; i < cols - 1; i++) {
        Zj[i] = 0.00;
        for (int j = 1; j < rows; j++) {
            Zj[i] += CB[j - 1] * table[j][i];
        }
        Zj_minus_Cj[i] = Zj[i] - table[0][i];  // Subtract to get Zj - Cj
    }

    for (int i = 0; i < cols - 1; i++) {
        if (Zj_minus_Cj[i] < minValue) {
            minValue = Zj_minus_Cj[i];
            pivotCol = i;
        }
    }
    return pivotCol;
}

int selectPivotRow(float table[MAX][MAX], int rows, int cols, int pivotCol) {
    float Ratio[MAX];
    int pivotRow = -1;
    float minRatio = 1e30;  // Large number for comparison

    for (int i = 1; i < rows; i++) {
        if (table[i][pivotCol] > 0) {  
            Ratio[i - 1] = table[i][cols - 1] / table[i][pivotCol];  // RHS / pivot column value
            if (Ratio[i - 1] < minRatio) {
                minRatio = Ratio[i - 1];
                pivotRow = i;
            }
        } else {
            Ratio[i - 1] = 1e30;  
        }
    }

    return pivotRow;
}

void pivot(float table[MAX][MAX], int pivotRow, int pivotCol, int rows, int cols) {
    float pivot = table[pivotRow][pivotCol];

    // Normalize the pivot row
    for (int j = 0; j < cols; j++) {
        table[pivotRow][j] /= pivot; 
    }

    // Update other rows
    for (int i = 1; i < rows; i++) {
        if (i != pivotRow) {
            float factor = table[i][pivotCol];
            for (int j = 0; j < cols; j++) {
                table[i][j] -= factor * table[pivotRow][j];
            }
        }
    }
}

void simplex(float table[MAX][MAX], int rows, int cols, int n, int m, float CB[MAX], int B[]) {
    int pivotCol, pivotRow;

    while(1){
        printTable(table, rows, cols, n, m, CB, B);
        pivotCol = selectPivotCol(table, rows, cols, CB);
        if (pivotCol == -1) {
            printf("Optimal Solution Found.\n");
            int optimal_sol = 0;
            for(int i=1;i<rows;i++){
                optimal_sol += CB[i-1] * table[i][cols-1];
                if(B[i-1] <= n){
                    printf("X%d = %.2f \n",B[i-1],CB[i-1]);
                }
            }
            printf("\nOptimal Solution = %d",optimal_sol);
            return;
        }
        pivotRow = selectPivotRow(table, rows, cols, pivotCol);
        if (pivotRow == -1) {
            printf("The Problem is Unbounded\n");
            return;
        }
        pivot(table, pivotRow, pivotCol, rows, cols);

        // Updating the B and CB columns
        int temp = B[pivotRow - 1];
        B[pivotRow - 1] = pivotCol + 1;
        CB[pivotRow - 1] = table[0][pivotCol];
    }
}

int main() {
    int m, n;  // Number of constraints (m) and variables (n)
    float A[MAX][MAX], b[MAX], c[MAX], CB[MAX];  
    float table[MAX][MAX]; 
    int B[MAX];
    int flag;
    
    printf("Enter the choice(MIN-0/MAX-1):");
    scanf("%d", &flag);
    int flagValue = 1;
    switch(flag){
        case 0:
            flagValue = -1;
            break;
        case 1:
            flagValue = 1;
            break;
    }

    printf("Enter the Number of Variables: ");
    scanf("%d", &n);
    
    printf("Enter the Number of Constraints: ");
    scanf("%d", &m);

    printf("Enter the Coefficients for Each Constraint:\n");
    for (int i = 0; i < m; i++) {
        printf("For Constraint %d:\n", i + 1);
        for (int j = 0; j < n; j++) {
            printf("A[%d][%d]: ", i + 1, j + 1);
            scanf("%f", &A[i][j]);
        }
    }

    printf("Enter the Coefficients of Variables in the Cost Function (Objective Function):\n");
    for (int i = 0; i < n; i++) {
        printf("Coefficient of x%d: ", i + 1);
        scanf("%f", &c[i]);
    }


    printf("Enter the RHS Value of each Constraint:\n");
    for (int i = 0; i < m; i++) {
        printf("b[%d]: ", i + 1);
        scanf("%f", &b[i]);
    }

    // Initialize CB and B
    for (int i = 0; i < m; i++) {
        CB[i] = 0;         
        B[i] = n + i + 1; 
    }

    // Initialize the table
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            table[i + 1][j] = A[i][j];  
        }
        for (int j = n; j < n + m; j++) {
            table[i + 1][j] = (j - n == i) ? 1 : 0;  
        }
        table[i + 1][n + m] = b[i];  
    }

    // Initialize the objective function row
    for (int j = 0; j < n; j++) {
        table[0][j] = c[j]*flag;    
    }
    for (int j = n; j < n + m; j++) {     // CB column - initialized with zeroes
        table[0][j] = 0;
    }
    table[0][n + m] = 0; // RHS of Objective Function is always zero

    simplex(table,m+1,n+m+1,n,m,CB,B);
    return 0;
}
