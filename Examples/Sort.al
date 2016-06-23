set A = {1, 4, 5, 6, 7, 215, 13, 8, 9, 0, -2, 111, -65, 3, -9, 12}

declare set A_s

declare int min

declare int i

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
		else { }
		
		let i = i + 1
	}
	
	let A_s = A_s U {min}

	let A = A \ {min}
}

print "Sorted Set: "

print A_s

quit