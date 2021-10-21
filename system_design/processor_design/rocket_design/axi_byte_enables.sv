

module axi_byte_enables
    (
        ADDR,
        SIZE,
        BE
    );

    input  [11:0]   ADDR;
    input  [2:0]    SIZE;
    output [7:0]    BE;
    reg    [7:0]    LOCAL_BE;

    assign BE = LOCAL_BE;

    always @(ADDR, SIZE) begin
    case (SIZE)
    3'b000   : begin
                 case (ADDR[2:0])
                 3'b000  : LOCAL_BE <= 8'b00000001;
                 3'b001  : LOCAL_BE <= 8'b00000010;
                 3'b010  : LOCAL_BE <= 8'b00000100;
                 3'b011  : LOCAL_BE <= 8'b00001000;
                 3'b100  : LOCAL_BE <= 8'b00010000;
                 3'b101  : LOCAL_BE <= 8'b00100000;
                 3'b110  : LOCAL_BE <= 8'b01000000;
                 3'b111  : LOCAL_BE <= 8'b10000000;
                 default : LOCAL_BE <= 8'b11111111;
                 endcase
              end
   
    3'b001   : begin
                 case (ADDR[2:0])
                 3'b000  : LOCAL_BE <= 8'b00000011;
                 3'b001  : LOCAL_BE <= 8'b00000110;
                 3'b010  : LOCAL_BE <= 8'b00001100;
                 3'b011  : LOCAL_BE <= 8'b00011000;
                 3'b100  : LOCAL_BE <= 8'b00110000;
                 3'b101  : LOCAL_BE <= 8'b01100000;
                 3'b110  : LOCAL_BE <= 8'b11000000;
                 3'b111  : LOCAL_BE <= 8'b10000000;
                 default : LOCAL_BE <= 8'b11111111;
                 endcase
              end
       
    3'b010   : begin
                 case (ADDR[2:0])
                 3'b000  : LOCAL_BE <= 8'b00001111;
                 3'b001  : LOCAL_BE <= 8'b00011110;
                 3'b010  : LOCAL_BE <= 8'b00111100;
                 3'b011  : LOCAL_BE <= 8'b01111000;
                 3'b100  : LOCAL_BE <= 8'b11110000;
                 3'b101  : LOCAL_BE <= 8'b11100000;
                 3'b110  : LOCAL_BE <= 8'b11000000;
                 3'b111  : LOCAL_BE <= 8'b10000000;
                 default : LOCAL_BE <= 8'b11111111;
                 endcase
              end
      
    default : begin
                 LOCAL_BE <= 8'b11111111;
              end
       
    endcase
    end
endmodule
