gpstk;

rin=gpstk.RinexObsStream("bahr1620.04o");
rout=gpstk.RinexObsStream("bahr1620.04o.new");
head=gpstk.RinexObsHeader();
data=gpstk.RinexObsData();

head.getRecord(rin);

rout.header=rin.header;

rout.header.putRecord(rout);

old=0;
new=1;

while (new > old)
old=rin.recordNumber();
data.getRecord(rin);
data.putRecord(rout);
new=rin.recordNumber();
end
