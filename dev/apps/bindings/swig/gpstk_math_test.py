import unittest
from gpstk import *


class SpecialFunctions_test(unittest.TestCase):
    def test(self):
        self.assertAlmostEqual(24.0, gamma(5))
        self.assertAlmostEqual(2.3632718012073, gamma(-1.5))
        self.assertAlmostEqual(0.8427007929497, erf(1))
        self.assertAlmostEqual(0.9661051464753, erf(1.5))


class Vector_test(unittest.TestCase):
    def test_standard_double(self):
        v = vector(5, 3.0)  # 3 3 3 3 3
        self.assertAlmostEqual(3.0, v[0])
        self.assertAlmostEqual(3.0, v[1])
        self.assertAlmostEqual(3.0, v[2])
        self.assertAlmostEqual(3.0, v[3])
        self.assertAlmostEqual(3.0, v[4])
        self.assertEqual(5, len(v))

    def test_from_stdvector_double(self):
        tmp = std_vector_double(5)
        tmp[0] = 1.23
        tmp[1] = 2.34
        tmp[2] = 3.45
        tmp[3] = 4.56
        tmp[4] = 5.67
        v = vector(tmp)
        self.assertAlmostEqual(1.23, v[0])
        self.assertAlmostEqual(2.34, v[1])
        self.assertAlmostEqual(3.45, v[2])
        self.assertAlmostEqual(4.56, v[3])
        self.assertAlmostEqual(5.67, v[4])

    def test_iter(self):
        v = vector(3, 2.5)
        i = 0
        for x in v:
            self.assertAlmostEqual(v[i], x)
            i += 1


class Stats_test(unittest.TestCase):
    def test_gaussian(self):
        s = GaussianDistribution(0, 1)
        self.assertAlmostEqual(.3989422804014327, s.pdf(0))
        self.assertAlmostEqual(.8413447460685429, s.cdf(1))

    def test_chi2(self):
        s = Chi2Distribution(5)
        self.assertAlmostEqual(.15418032980376903, s.pdf(3))
        self.assertAlmostEqual(.45058404864721940, s.cdf(4))

    def test_student(self):
        s = StudentDistribution(4)
        self.assertAlmostEqual(.01969349809083655, s.pdf(3))
        self.assertAlmostEqual(.94194173824159210, s.cdf(2))

    def test_stats(self):
        tmp = std_vector_double(3);
        tmp[0] = 5.0
        tmp[1] = 4.5
        tmp[2] = 6.0
        v = vector(tmp)
        w = vector(3, 1.0)
        s = Stats_double(v, w)
        self.assertAlmostEqual(4.5, s.Minimum())
        self.assertAlmostEqual(5.166666666666667, s.Average())
        self.assertAlmostEqual(0.583333333333333, s.Variance())

        s.Add(10.0, 2.0)  # add a weighted value
        self.assertAlmostEqual(10.0, s.Maximum())
        self.assertAlmostEqual(7.1, s.Average())
        self.assertAlmostEqual(7.786666666666663, s.Variance())

    def test_twoStats(self):
        s = TwoSampleStats_double()
        # sample forms y = 2x + 500
        s.Add(1000.0, 2500.0)
        s.Add(2000.0, 4500.0)
        s.Add(3000.0, 6500.0)
        # x vals
        self.assertAlmostEqual(1000.0, s.MinimumX())
        self.assertAlmostEqual(3000.0, s.MaximumX())
        self.assertAlmostEqual(2000.0, s.AverageX())
        self.assertAlmostEqual(1000000.0, s.VarianceX())
        self.assertAlmostEqual(1000.0, s.StdDevX())

        # y vals
        self.assertAlmostEqual(2500.0, s.MinimumY())
        self.assertAlmostEqual(6500.0, s.MaximumY())
        self.assertAlmostEqual(4500.0, s.AverageY())
        self.assertAlmostEqual(4000000.0, s.VarianceY())
        self.assertAlmostEqual(2000.0, s.StdDevY())

        # x and y
        self.assertAlmostEqual(2.0, s.Slope())
        self.assertAlmostEqual(500.0, s.Intercept())
        self.assertAlmostEqual(8.940696716308592e-08, s.SigmaSlope())
        self.assertAlmostEqual(1.0, s.Correlation())
        self.assertAlmostEqual(0.0001264405455326821, s.SigmaYX())


if __name__ == '__main__':
    unittest.main()
