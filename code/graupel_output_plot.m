function graupel_output_plot(fig,filename,marker)
	data=csvread2(filename,'\t');
	D_max=data.D_max;
	V=data.Volume;
	V_ball=4*pi*(0.5*D_max).^3/3;
	plot(fig,D_max,V./V_ball,marker);
end