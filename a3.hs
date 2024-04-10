-- CISC 360 a3, Fall 2023

-- SEE THE FILE a3.pdf
-- for instructions

module A3 where

import Data.List
import Distribution.Simple.Setup (BooleanFlag)

-- Rename this file to include your student ID: a3-studentid.hs
-- Also, add your student ID number after the "=":
student_id :: Integer
student_id = 20332619

{- If you are working in a group of 2, uncomment the line below and add the second student's ID number: -}
-- second_student_id =
{- If you are working in a group of 2, uncomment the line above. -}

-- THIS FILE WILL NOT COMPILE UNTIL YOU ADD YOUR STUDENT ID ABOVE
check_that_you_added_your_student_ID_above = ()

-- Variable is a synonym for String.
type Variable = String

-- In our simplified version of classical propositional logic,
-- we have the following definition for a Formula:
data Formula
  = Top -- truth (always true)
  | Bot -- falsehood (contradiction)
  | And Formula Formula -- conjunction
  | Or Formula Formula -- disjunction
  | Implies Formula Formula -- implication
  | Not Formula -- negation
  | Atom Variable -- atomic proposition ("propositional variable")
  deriving (Eq, Show)

-- Some atoms, for convenience
vA = Atom "A"

vB = Atom "B"

vC = Atom "C"

vD = Atom "D"

vE = Atom "E"

vF = Atom "F"

vp = Atom "p"

vq = Atom "q"

-- Some example formulas that you can use to test your functions
formula1 = Implies (And vA vB) vC

formula2 = Implies Bot (And vA vB)

formula3 = Implies (And vA vB) Top

formula4 = And (Implies vA (And vB vC)) (And vD vE)

formula5 = And vA vB

formula6 = Not vA

formula7 = Implies vA vB

formula8 = Or vA (Not vA)

formula9 = Or vA (Not vB)


{-
Q2: Truth Tables

To build a truth table for a formula, there are 4 steps:

  1) Traverse the formula to find all atomic propositions (propositional variables).

  2) Find all the possible valuations---combinations of True and False
      for the atomic propositions in the formula.

  3) Evaluate the formula for each valuation obtained in (2).

  4) Use the results of (1-3) to build the table.

In this question, you will implement steps (1-3).
-}

-- A Valuation is a list of pairs corresponding to a truth value (i.e. True or False)
--  for each atom in a formula
type Valuation = [(Variable, Bool)]

-- A TruthTable is an enumeration of the valuations for a given formula,
-- with each valuation paired with the corresponding evaluation of that formula.
-- (This corresponds to a truth table with no "intermediate columns".)
data TruthTable = TruthTable [(Valuation, Bool)]

{-
   This function is here so that when you print a TruthTable in GHCi, the table is nice and readable.
   You don't need to understand how this works to complete the assignment.
-}
instance Show TruthTable where
  show (TruthTable rows) =
    case rows of
      [] -> ""
      ([], result) : _ -> "   result is " ++ pad_show result ++ "\n"
      ((c, b) : valu, result) : xs ->
        c
          ++ "="
          ++ (pad_show b)
          ++ "   "
          ++ show (TruthTable [(valu, result)])
          ++ show (TruthTable xs)
    where
      pad_show True = "True "
      pad_show False = "False"

{-
  Q2a: getVariables:

  Traverse a formula and build a list of all Variables in the formula, without duplicates.

  You may use the built-in function "nub", which takes a list and returns the list
  without duplicates.
-}
getVariables :: Formula -> [Variable]
getVariables Top = []
getVariables Bot = []
getVariables (Atom v) = [v]
getVariables (Not phi) = nub (getVariables phi)
getVariables (And phi1 phi2) = nub (getVariables phi1 ++ getVariables phi2)
getVariables (Or phi1 phi2) = nub (getVariables phi1 ++ getVariables phi2)
getVariables (Implies phi psi) = nub (getVariables phi ++ getVariables psi)

{-
   Q2b: getValuations:

   Build a list of all possible valuations for a set of atomic propositions
-}

getValuations :: [Variable] -> [Valuation]
getValuations [] = [[]]
getValuations (c : cs) = if length cs == 0 then [[(c, True)], [(c, False)]] else (map (\k -> (c, True) : k) (getValuations cs)) ++ (map (\k -> (c, False) : k) (getValuations cs))

{-
  Hint: To apply a function f to every element of a list xs,
   write  map f xs.
  For example, the following adds 1 to the start of every list
   in a list of lists [[2,3], [2,4]]:
   map (\ys -> 1 : ys) [[2,3], [2,4]]  ==  [[1,2,3], [1,2,4]]
-}

{-
    Helper function check to see if a atom with sepcific value exists in a valuation.
-}
checkElement :: (Variable, Bool) -> Valuation -> Bool
checkElement _ [] = False
checkElement x (c : cs) = if c == x then True else (checkElement x cs)

{-
   Q2c: evalF:
    Evaluate a formula with a particular valuation,
     returning the resulting boolean value
-}
evalF :: Valuation -> Formula -> Bool
evalF valu formula =
  case formula of
    Top -> True
    Bot -> False
    Not phi1 -> not (evalF valu phi1)
    Implies phi1 phi2 -> not (evalF valu phi1) || (evalF valu phi2)
    Atom c -> (checkElement (c, True) valu)
    And phi1 phi2 -> (evalF valu phi1) && (evalF valu phi2)
    Or phi1 phi2 -> (evalF valu phi1) || (evalF valu phi2)

-- buildTable:
--  Build a truth table for a given formula.
--  You can use this function to help check your definitions
--  of getVariables, getValuations and evalF.
buildTable :: Formula -> TruthTable
buildTable psi =
  let valuations = getValuations (getVariables psi)
   in TruthTable
        ( zip
            valuations
            (map (\valu -> evalF valu psi) valuations)
        )