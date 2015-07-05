;
;  pictures.s
;
;  Author: Kestutis Bivainis
;
	AREA MY, DATA, READONLY
	EXPORT data1
	EXPORT data2
	EXPORT data3
	EXPORT data4
	EXPORT data5
data1
	INCBIN ..\Pictures\1.jpg
data2
	INCBIN ..\Pictures\2.jpg
data3
	INCBIN ..\Pictures\3.jpg
data4
	INCBIN ..\Pictures\4.jpg
data5
	INCBIN ..\Pictures\5.jpg
	END
