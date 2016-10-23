set A = {1, 4, 5, 6, 7, 215, 13, 8, 9, 0, -2, 111, -65, 3, -9, 12}
declare set A_s
declare ints min, i
while (|A| > 0)						
{
	let min = A[0]
	let i = 1
	while (i < |A|)
	{
		if (min > A[i])
		{
			let min = A[i]	
		}	
		let i += 1
	}
	let A \= { min } 
	let A_s U= { .min }
}
print "Sorted Set:", A_s
# {-65, -9, -2, 0, 1, 3, 4, 5, 6, 7, 8, 9, 12, 13, 111, 215}
quit
