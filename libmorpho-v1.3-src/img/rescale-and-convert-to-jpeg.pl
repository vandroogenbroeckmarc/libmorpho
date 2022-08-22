#!/usr/bin/perl

@files = glob("*\.pgm");
foreach $i (@files)
	{
	# Rescaling
	print "Rescaling (downscaling by 60%) $i ...\n";
	@name = split('\.',$i);
	system("convert -scale 40% $i $name[0]"."\.jpg");
	}

