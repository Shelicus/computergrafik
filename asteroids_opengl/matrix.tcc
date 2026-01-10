#include <cassert>

// Konstruktor mit Initializer List
template <class FLOAT, size_t N>
SquareMatrix<FLOAT, N>::SquareMatrix(std::initializer_list< Vector<FLOAT, N > > values) {
  size_t i = 0;
  for (const auto& val : values) {
    if (i < N) {
      matrix[i++] = val;
    }
  }
}

// Zugriff auf Spalten-Referenz
template <class FLOAT, size_t N>
Vector<FLOAT, N> & SquareMatrix<FLOAT, N>::operator[](std::size_t i) {
  return matrix[i];
}

// Zugriff auf Spalten-Kopie (const)
template <class FLOAT, size_t N>
Vector<FLOAT, N> SquareMatrix<FLOAT, N>::operator[](std::size_t i) const {
  return matrix[i];
}

// Zugriff auf Wert an (Zeile, Spalte) - const
template <class FLOAT, size_t N>
FLOAT SquareMatrix<FLOAT, N>::at(size_t row, size_t column) const {
  assert(row < N && column < N); 
  return matrix[column][row];
}

// Zugriff auf Wert an (Zeile, Spalte) - Referenz
template <class FLOAT, size_t N>
FLOAT & SquareMatrix<FLOAT, N>::at(size_t row, size_t column) {
  assert(row < N && column < N);
  return matrix[column][row];
}

// Matrix-Vektor Multiplikation: y = A * x
template <class FLOAT, size_t N>
Vector<FLOAT,N> SquareMatrix<FLOAT, N>::operator*(const Vector<FLOAT,N> vector) {
  Vector<FLOAT, N> result = vector[0] * matrix[0]; 

  for (size_t i = 1; i < N; ++i) {
    result += vector[i] * matrix[i];
  }
  return result;
}

// Matrix-Matrix Multiplikation: C = A * B
template <class F, size_t K>
SquareMatrix<F, K> operator*(const SquareMatrix<F, K> factor1, const SquareMatrix<F, K> factor2) {
  SquareMatrix<F, K> product;

  for (size_t i = 0; i < K; ++i) {
    
    Vector<F, K> col_result = factor2[i][0] * factor1[0];
    for (size_t k = 1; k < K; ++k) {
      col_result += factor2[i][k] * factor1[k]; //Nutze Matrix-Vektor Multiplikation
    }
    
    product[i] = col_result;
  }
  
  return product;
}