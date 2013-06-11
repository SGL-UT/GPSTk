from gpstk_time import *

fails = []

def test(actual, expected=True, name=''): 
	print 'TEST:', name
	if actual != expected: 
		fails.append(name)
		print 'Test failure:'
		print 'Expected output:'
		print expected
		print 'Actual output:',
		print actual
		print ''
		print ''
	else:
		print '\tPASS'


timeSystem = TimeSystem()
timeSystem.setTimeSystem(TimeSystem.GPS)


t1 = CommonTime(timeSystem)
t1.addSeconds(100)
t2 = CommonTime(timeSystem)
t2.addSeconds(500)
test(t1 <  t2, True,  't1 < t2')
test(t1 <= t2, True,  't1 <= t2')
test(t1 >  t2, False, 't1 > t2')
test(t1 >= t2, False, 't1 >= t2')
test(t1 == t2, False, 't1 == t2')
test(t1 != t2, True,  't1 != t2')


t3 = UnixTime(0) # jan 1 1970
t3.setTimeSystem(timeSystem)
t4 = SystemTime() # hopefuly the year is 20xx
t4.setTimeSystem(timeSystem)
test(t3 <  t4, True,  't3 < t4')
test(t3 <= t4, True,  't3 <= t4')
test(t3 >  t4, False, 't3 > t4')
test(t3 >= t4, False, 't3 >= t4')
test(t3 == t4, False, 't3 == t4')
test(t3 != t4, True,  't3 != t4')


t5 = CivilTime(2012, 11, 6, 20, 40, 400).convertToCommonTime() # in 2012
t5.setTimeSystem(timeSystem)
t6 = UnixTime(1370983244, 659200).convertToCommonTime() # in 2013
t6.setTimeSystem(timeSystem)
test(t5 <  t6, True,  't5 < t6')
test(t5 <= t6, True,  't5 <= t6')
test(t5 >  t6, False, 't5 > t6')
test(t5 >= t6, False, 't5 >= t6')
test(t5 == t6, False, 't5 == t6')
test(t5 != t6, True,  't5 != t6')


t7 = CommonTime(timeSystem)
t7.addDays(3)
t8 = CommonTime(timeSystem)
t8.addSeconds(60*60*24*3)
test(t7 <  t8, False, 't7 < t8')
test(t7 <= t8, True,  't7 <= t8')
test(t7 >  t8, False, 't7 > t8')
test(t7 >= t8, True,  't7 >= t8')
test(t7 == t8, True,  't7 == t8')
test(t7 != t8, False, 't7 != t8')


print ''
print 'Number of fails:', len(fails)
print 'Test fails:', fails
