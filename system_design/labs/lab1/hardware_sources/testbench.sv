`define reset_count  (10)
`define clock_delay  (`CLOCK_PERIOD / 2)

module testbench;
   
   logic clock      = 1'b1;
   logic reset_bar  = 1'b1;
   int i;

   initial begin
      reset_bar = 1'b0;
      for (i=0; i<`reset_count; i++) @(posedge clock);
      reset_bar = 1'b1;
   end

   always #`clock_delay clock = ~clock;

   top top(
     .clk       (clock),
     .reset_bar (reset_bar)
   );

endmodule
