import unittest
from gpstk import *

timeSystem = TimeSystem()
timeSystem.setTimeSystem(TimeSystem.GPS)

class time_comparison1(unittest.TestCase):
	def test(self):		
		t1 = CommonTime(timeSystem)
		t1.addSeconds(100)
		t2 = CommonTime(timeSystem)
		t2.addSeconds(500)
		self.assertEqual(t1 <  t2, True)
		self.assertEqual(t1 <  t2, True)
		self.assertEqual(t1 <= t2, True)
		self.assertEqual(t1 >  t2, False)
		self.assertEqual(t1 >= t2, False)
		self.assertEqual(t1 == t2, False)
		self.assertEqual(t1 != t2, True)

class time_comparison2(unittest.TestCase):
	def test(self):		
		t3 = UnixTime(0) # jan 1 1970
		t3.setTimeSystem(timeSystem)
		t4 = SystemTime() # hopefuly the year is 20xx
		t4.setTimeSystem(timeSystem)
		self.assertEqual(t3 <  t4, True)
		self.assertEqual(t3 <= t4, True)
		self.assertEqual(t3 >  t4, False)
		self.assertEqual(t3 >= t4, False)
		self.assertEqual(t3 == t4, False)
		self.assertEqual(t3 != t4, True)

class time_comparison3(unittest.TestCase):
	def test(self):		
		t5 = UnixTime(1370983244, 659200).convertToCommonTime() # in 2013
		t5.setTimeSystem(timeSystem)
		t6 = CivilTime(2012, 11, 6, 20, 40, 400).convertToCommonTime() # in 2012
		t6.setTimeSystem(timeSystem)
		self.assertEqual(t5 <  t6, False)
		self.assertEqual(t5 <= t6, False)
		self.assertEqual(t5 >  t6, True)
		self.assertEqual(t5 >= t6, True)
		self.assertEqual(t5 == t6, False)
		self.assertEqual(t5 != t6, True)

class time_comparison4(unittest.TestCase):
	def test(self):		
		t7 = CommonTime(timeSystem)
		t7.addDays(3)
		t8 = CommonTime(timeSystem)
		t8.addSeconds(60*60*24*3)
		self.assertEqual(t7 <  t8, False)
		self.assertEqual(t7 <= t8, True)
		self.assertEqual(t7 >  t8, False)
		self.assertEqual(t7 >= t8, True)
		self.assertEqual(t7 == t8, True)
		self.assertEqual(t7 != t8, False)

if __name__ == '__main__':
	unittest.main()
