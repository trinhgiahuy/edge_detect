
module sram
    (
        CLK,
        RSTn,
        READ_ADDR,
        DATA_OUT,
        OE,
        WRITE_ADDR,
        DATA_IN,
        BE,
        WE
    );

    parameter address_width     = 22;
    parameter data_width        = 2; //   in 2^data_width bytes
                                     //   0 = 8 bits  1 = 16 bits  2 = 32 bits  3 = 64 bits
                                     //   memory is always byte addressible

    input                                CLK;
    input                                RSTn;
    input [address_width-1:0]            READ_ADDR;
    output [((1<<data_width)*8)-1:0]     DATA_OUT;
    input                                OE;
    input [address_width-1:0]            WRITE_ADDR;
    input [((1<<data_width)*8)-1:0]      DATA_IN;
    input [(1<<data_width)-1:0]          BE;
    input                                WE;
   
    
    reg [((1<<data_width)*8)-1:0] mem [(1<<address_width-data_width)-1:0];
    reg [((1<<data_width)*8)-1:0] read_data;

    genvar w;

    assign DATA_OUT = read_data;

    always @(posedge CLK) begin
        if (RSTn == 1'b0) begin
            read_data <= { ((1<<data_width) * 8) {1'b0}};
        end else begin
            if (OE) begin
                read_data <= mem[READ_ADDR];
            end
        end
    end

    generate 
        for (w=0; w<(1<<data_width); w = w + 1) 
            always @(posedge CLK) begin
                if (WE && BE[w]) mem[WRITE_ADDR][((w+1)*8)-1:(w*8)] <= DATA_IN[((w+1)*8)-1:(w*8)];
            end
    endgenerate

endmodule
