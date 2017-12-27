program factorial  
implicit none  

   ! define variables
   integer :: nfact = 1   
   integer :: n = 1 
   
   ! compute factorials   
   do while (n <= 10)       
      nfact = nfact * n 
      n = n + 1
      print*,  n, " ", nfact   
   end do 
end program factorial
