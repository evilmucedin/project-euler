{-# LANGUAGE ScopedTypeVariables #-}
{-# LANGUAGE PostfixOperators #-}

{- Let's call a lattice point (x, y) inadmissible if x, y and x + y are all
positive perfect squares. For example, (9, 16) is inadmissible, while (0, 4),
(3, 1) and (9, 4) are not.

Consider a path from point (x1, y1) to point (x2, y2) using only unit steps
north or east. Let's call such a path admissible if none of its intermediate
points are inadmissible.

P(n) be the number of admissible paths from (0, 0) to (n, n).
It can be verified that P(5) = 252, P(16) = 596994440 and
P(1000) mod 1 000 000 007 = 341920854.

Find P(10 000 000) mod 1 000 000 007.

-------------------------------------

Runs in approximately 1m3s. Compile and run with
	
	$ ghc -O2 408.hs; time ./408

Solution by Jack Wines and Brett Wines on December 26, 2014 -}

import qualified Zora.Math as ZM
import qualified Data.List as List
import qualified Data.Array.Unboxed as UArray
import qualified Data.MemoCombinators as Memo

type Point = (Int, Int)

-- Constants --

p :: Int
p = 1000000007

n :: Int
n = 10000000

-- Modular Arithmetic --

(|+|) :: Int -> Int -> Int
a |+| b = ZM.add_mod p a b

(|-|) :: Int -> Int -> Int
a |-| b = ZM.sub_mod p a b

(|*|) :: Int -> Int -> Int
a |*| b = ZM.mul_mod p a b

(|/|) :: Int -> Int -> Int
a |/| b = ZM.div_mod_prime p a b

sum_mod_p :: [Int] -> Int
sum_mod_p = List.foldl' (|+|) 0

-- Logic --

inadmissable_points :: [Point]
inadmissable_points =
	filter is_inadmissable_point [(x,y) | x <- squares, y <- squares]
	where
		squares :: [Int]
		squares = takeWhile (<= n) . map (^2) $ [1..]

		is_inadmissable_point :: Point -> Bool
		is_inadmissable_point (x, y) = 
			   (ZM.square (x + y))
			&& (ZM.square x)
			&& (ZM.square y)

-- The number of paths between the two given points is given by the following
-- formula: of the number of steps to take in the path (which is constant
-- irrespective of the path taken), choose the ones to be in (w.l.o.g.) the
-- x direction.
num_paths_between :: Point -> Point -> Int
num_paths_between (i, j) (x, y) = num_steps `choose` (abs $ x |-| i)
	where
		-- steps in x direction + steps in y direction
		num_steps :: Int
		num_steps = (abs $ x |-| i) |+| (abs $ y |-| j)

		choose :: Int -> Int -> Int
		choose n k = (n!) |/| ((k!) |*| ((n |-| k)!))

		-- Unboxed array for (much) faster access
		factorials :: UArray.UArray Int Int
		factorials = UArray.array (0, 2*n)
			$ zip [0..] $ scanl (|*|) 1 [1..2*n]

		(!) :: Int -> Int
		(!) n = factorials UArray.! n

-- Memoization and symmetry optimization for `num_admissable_paths_from`.
num_admissable_paths_from :: Point -> Int
num_admissable_paths_from point =
	Memo.pair Memo.integral Memo.integral num_admissable_paths_from' point'
	where
		-- The definition of inadmissability is symmetric in `x` and `y`,
		-- so w.l.o.g. we can compute only one half of the inadmissable points.
		point' :: Point
		point' = if (fst point) < (snd point)
			then point
			else (snd point, fst point)

-- Counts the number of inadmissable paths to (n, n) from the given point.
num_admissable_paths_from' :: Point -> Int
num_admissable_paths_from' point = total_num_paths |-| num_inadmissable_paths
	where
		num_inadmissable_paths :: Int
		num_inadmissable_paths
			= sum_mod_p
			. map (num_last_inadmissable_point_paths point)
			$ inadmissable_points_after point

		total_num_paths :: Int
		total_num_paths = num_paths_between point (n, n)

		-- Counts the number of paths from `start` through `middle`
		-- for which `middle` is the last inadmissable point
		num_last_inadmissable_point_paths :: Point -> Point -> Int
		num_last_inadmissable_point_paths start middle
			= (num_paths_between start middle)
			|*| (num_admissable_paths_from middle)

		inadmissable_points_after :: Point -> [Point]
		inadmissable_points_after p@(x, y) = filter after inadmissable_points
			where
				after :: Point -> Bool
				after p2@(a, b) = (a >= x && b >= y) && (p /= p2)

main :: IO ()
main = do
	putStrLn . show $ n
	putStrLn . show $ num_admissable_paths_from (0, 0)

