#include "matrix.h"
#include "gtest/gtest.h"

namespace {
	
TEST(MATRIX, ListInitialization2df) {
  SquareMatrix2df matrix = { Vector2df{1.0, 0.0},
                             Vector2df{0.0, 1.0} };
  
  EXPECT_NEAR(1.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(1,0), 0.00001);
  EXPECT_NEAR(1.0, matrix.at(1,1), 0.00001);
}

TEST(MATRIX, ListInitialization3df) {
  SquareMatrix3df matrix = { Vector3df{1.0, 0.0, 0.0},
                             Vector3df{0.0, 1.0, 0.0},
                             Vector3df{0.0, 0.0, 1.0} };
                               
  EXPECT_NEAR(1.0, matrix[0][0], 0.00001);
  EXPECT_NEAR(0.0, matrix[0][1], 0.00001);
  EXPECT_NEAR(0.0, matrix[0][2], 0.00001);
  EXPECT_NEAR(0.0, matrix[1][0], 0.00001);
  EXPECT_NEAR(1.0, matrix[1][1], 0.00001);
  EXPECT_NEAR(0.0, matrix[1][2], 0.00001);
  EXPECT_NEAR(0.0, matrix[2][0], 0.00001);
  EXPECT_NEAR(0.0, matrix[2][1], 0.00001);
  EXPECT_NEAR(1.0, matrix[2][2], 0.00001);
}

TEST(MATRIX, ListInitialization4df) {
  SquareMatrix4df matrix = { {1.0, 5.0, 9.0, 13.0},
                             {2.0, 6.0, 10.0, 14.0},
                             {3.0, 7.0, 11.0, 15.0},
                             {4.0, 8.0, 12.0, 16.0} };
  
  float v = 1.0f;
  for (size_t row = 0; row < 4; row++) {
    for (size_t column = 0;  column < 4; column++) {    
      EXPECT_NEAR(v++, matrix.at(row, column), 0.00001);
    }
  }
}



TEST(MATRIX, ProductWithVector3df) {
  SquareMatrix3df matrix = { {1.0, 0.0, 1.0},
                             {0.0, 1.0, 1.0},
                             {0.0, 0.0, 1.0} };
  Vector3df vector = {-6.0, 3.0,  1.0};
  Vector3df product = matrix * vector;  
  
  EXPECT_NEAR(-6.0, product[0], 0.00001);
  EXPECT_NEAR( 3.0, product[1], 0.00001);
  EXPECT_NEAR(-2.0, product[2], 0.00001);
}



TEST(MATRIX, ProductWithMatrix2df) {
  SquareMatrix3df matrix1 = { {1.0, 2.0},
                              {-1.0, 1.5} };
  SquareMatrix3df matrix2 = { {2.0, -1.0},
                              {1.0, 0.0} };
  SquareMatrix3df matrix = matrix1 * matrix2; 
  
  EXPECT_NEAR( 3.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR( 1.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR( 2.5, matrix.at(1,0), 0.00001);
  EXPECT_NEAR( 2.0, matrix.at(1,1), 0.00001);
}

//Eigenen Tests:
//Inital
TEST(MATRIX, Initialization2df_ColumnMajor) {
  
  SquareMatrix2df matrix = { Vector2df{1.0, 2.0},
                             Vector2df{3.0, 4.0} };
  
  EXPECT_NEAR(1.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR(3.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR(2.0, matrix.at(1,0), 0.00001);
  EXPECT_NEAR(4.0, matrix.at(1,1), 0.00001);
}

TEST(MATRIX, Initialization3df_Identity) {
  SquareMatrix3df matrix = { Vector3df{1.0, 0.0, 0.0},
                             Vector3df{0.0, 1.0, 0.0},
                             Vector3df{0.0, 0.0, 1.0} };
                             

  EXPECT_NEAR(1.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR(1.0, matrix.at(1,1), 0.00001);
  EXPECT_NEAR(1.0, matrix.at(2,2), 0.00001);
  
  EXPECT_NEAR(0.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(1,0), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(2,0), 0.00001);
}

//Zugriffs-Tests at
TEST(MATRIX, AccessAndModification_at) {
  SquareMatrix2df matrix = { {0.0, 0.0}, {0.0, 0.0} };
  matrix.at(0, 1) = 5.0; 
  matrix.at(1, 0) = 3.0;


  EXPECT_NEAR(5.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR(3.0, matrix.at(1,0), 0.00001);

  EXPECT_NEAR(0.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(1,1), 0.00001);
}

TEST(MATRIX, AccessAndModification_BracketOperator) {
  SquareMatrix2df matrix = { {0.0, 0.0}, {0.0, 0.0} };

  matrix[0][1] = 9.0; 

  matrix[1][0] = 7.0;

  EXPECT_NEAR(9.0, matrix.at(1,0), 0.00001);
  EXPECT_NEAR(7.0, matrix.at(0,1), 0.00001);
}

//Matrix-Vektor Multiplikation
TEST(MATRIX, ProductWithVector3df_General) {
  SquareMatrix3df matrix = { {1.0, 0.0, 0.0},
                             {0.0, 1.0, 0.0},
                             {1.0, 1.0, 1.0} };
                             
  Vector3df vector = {-6.0, 3.0, 1.0};
  
  Vector3df product = matrix * vector;  
  
  EXPECT_NEAR(-5.0, product[0], 0.00001);
  EXPECT_NEAR( 4.0, product[1], 0.00001);
  EXPECT_NEAR( 1.0, product[2], 0.00001);
}

TEST(MATRIX, ProductWithVector2df_Scaling) {

  SquareMatrix2df matrix = { {2.0, 0.0}, {0.0, 2.0} };
  Vector2df vector = {3.0, 4.0};

  Vector2df product = matrix * vector;

  EXPECT_NEAR(6.0, product[0], 0.00001);
  EXPECT_NEAR(8.0, product[1], 0.00001); 
}

//Matrix-Matrix Multiplikation
TEST(MATRIX, ProductWithMatrix2df_IdentityCheck) {
  SquareMatrix2df A = { {10.0, 30.0}, {20.0, 40.0} }; 
                                                      
                                                      
  SquareMatrix2df I = { {1.0, 0.0}, {0.0, 1.0} };   

  SquareMatrix2df Result = A * I;

  EXPECT_NEAR(10.0, Result.at(0,0), 0.00001);
  EXPECT_NEAR(20.0, Result.at(0,1), 0.00001);
  EXPECT_NEAR(30.0, Result.at(1,0), 0.00001);
  EXPECT_NEAR(40.0, Result.at(1,1), 0.00001);
}

TEST(MATRIX, ProductWithMatrix2df_Calculation) {
  SquareMatrix2df A = { {1.0, 3.0}, {2.0, 4.0} };


  SquareMatrix2df B = { {2.0, 1.0}, {0.0, 2.0} };
    SquareMatrix2df C = A * B; 
  
  EXPECT_NEAR( 4.0, C.at(0,0), 0.00001);
  EXPECT_NEAR( 4.0, C.at(0,1), 0.00001);
  EXPECT_NEAR(10.0, C.at(1,0), 0.00001);
  EXPECT_NEAR( 8.0, C.at(1,1), 0.00001);
}

TEST(MATRIX, ProductWithMatrix3df_ZeroCheck) {
  // A * 0 = 0
  SquareMatrix3df A = { {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0} };
  SquareMatrix3df Zero = { {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0} };

  SquareMatrix3df Result = A * Zero;

  for(int r=0; r<3; r++) {
      for(int c=0; c<3; c++) {
          EXPECT_NEAR(0.0, Result.at(r,c), 0.00001);
      }
  }
}

}
