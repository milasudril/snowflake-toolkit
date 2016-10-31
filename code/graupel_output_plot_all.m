a=axes();
hold on;
graupel_output_plot(a,'graupel-output/0-40-1.txt','r');
graupel_output_plot(a,'graupel-output/0-40-2.txt','g');
graupel_output_plot(a,'graupel-output/0-40-3.txt','b');
graupel_output_plot(a,'graupel-output/0-40-4.txt','k');
ylim([0 0.5]);
xlabel('D_{max}');
ylabel('Fill ratio');
title('E_0=0')

figure();
a=axes();
hold on
graupel_output_plot(a,'graupel-output/12-40-1.txt','r');
graupel_output_plot(a,'graupel-output/12-40-2.txt','g');
graupel_output_plot(a,'graupel-output/12-40-3.txt','b');
graupel_output_plot(a,'graupel-output/12-40-4.txt','k');
ylim([0 0.5]);
xlabel('D_{max}');
ylabel('Fill ratio');
title('E_0=12, decay distance=40');

figure();
a=axes();
hold on
graupel_output_plot(a,'graupel-output/12-50-1.txt','r');
graupel_output_plot(a,'graupel-output/12-50-2.txt','g');
graupel_output_plot(a,'graupel-output/12-50-3.txt','b');
graupel_output_plot(a,'graupel-output/12-50-4.txt','k');
ylim([0 0.5]);
xlabel('D_{max}');
ylabel('Fill ratio');
title('E_0=12, decay distance=50');

figure();
a=axes();
hold on
graupel_output_plot(a,'graupel-output/12-30-1.txt','r');
graupel_output_plot(a,'graupel-output/12-30-2.txt','g');
graupel_output_plot(a,'graupel-output/12-30-3.txt','b');
graupel_output_plot(a,'graupel-output/12-30-4.txt','k');
ylim([0 0.5]);
xlabel('D_{max}');
ylabel('Fill ratio');
title('E_0=12, decay distance=30');



figure();
a=axes();
hold on
graupel_output_plot(a,'graupel-output/6-40-1.txt','r');
graupel_output_plot(a,'graupel-output/6-40-2.txt','g');
graupel_output_plot(a,'graupel-output/6-40-3.txt','b');
graupel_output_plot(a,'graupel-output/6-40-4.txt','k');
ylim([0 0.5]);
xlabel('D_{max}');
ylabel('Fill ratio');
title('E_0=6, decay distance=40');

figure();
a=axes();
hold on
graupel_output_plot(a,'graupel-output/6-50-1.txt','r');
graupel_output_plot(a,'graupel-output/6-50-2.txt','g');
graupel_output_plot(a,'graupel-output/6-50-3.txt','b');
graupel_output_plot(a,'graupel-output/6-50-4.txt','k');
ylim([0 0.5]);
xlabel('D_{max}');
ylabel('Fill ratio');
title('E_0=6, decay distance=50');

figure();
a=axes();
hold on
graupel_output_plot(a,'graupel-output/6-30-1.txt','r');
graupel_output_plot(a,'graupel-output/6-30-2.txt','g');
graupel_output_plot(a,'graupel-output/6-30-3.txt','b');
graupel_output_plot(a,'graupel-output/6-30-4.txt','k');
ylim([0 0.5]);
xlabel('D_{max}');
ylabel('Fill ratio');
title('E_0=6, decay distance=30');