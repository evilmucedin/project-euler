     PROGRAM bigsum
        IMPLICIT NONE
        INTEGER :: n
        CHARACTER(LEN=1000) :: line
        INTEGER, PARAMETER :: MAXLEN = 1000
        INTEGER, DIMENSION(0:MAXLEN - 1) :: num
        INTEGER :: beginI
        INTEGER :: endI
        INTEGER :: endS
        INTEGER :: i
        INTEGER :: j
        INTEGER :: k
        INTEGER :: overflow

        DO i = 0, MAXLEN - 1
           num(i) = 0
        END DO

        READ *, n
        READ '(A)', line
        beginI = 0
        endI = 0
        endS = LEN(line)
        i = 0
        DO i = 1, n
           beginI = endI + 1
           endI = beginI + 1
           DO WHILE ((endI < endS) .AND. (line(endI:endI) /= " "))
              endI = endI + 1
           END DO
           k = 0
           DO j = endI - 1, beginI, -1
              num(k) = num(k) + (IACHAR(line(j:j)) - IACHAR("0"))
              k = k + 1
           END DO
        END DO

        overflow = 0
        DO i = 0, MAXLEN - 1
           num(i) = num(i) + overflow
           overflow = num(i)/10
           num(i) = MOD(num(i), 10)
        END DO

        j = MAXLEN - 1
        DO WHILE (num(j) == 0)
           j = j - 1
        END DO

        DO WHILE (j >= 0)
           WRITE (*, "(I0)", advance="no") num(j)
           j = j - 1
        END DO
        PRINT '(A)', ""

     END PROGRAM bigsum
