import unittest, sys, os
sys.path.insert(0, os.path.abspath(".."))
from gpstk.test_utils import args,run_unit_tests
import gpstk

class TestPRSolve(unittest.TestCase):

    def test_pr_solution_constructor(self):
        prs = gpstk.PRSolution()
        self.assertEqual(True, False)


if __name__ == '__main__':
    unittest.main()
