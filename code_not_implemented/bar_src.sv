module bar_src (
   input  logic clk,
   input  logic [10:0] x, y,     // treated as x-/y-axis
   output logic [11:0] bar_rgb 
   );

   // signal declaration
   //logic [3:0] up, down;
   //logic [3:0] r, g, b;
   logic [11:0] rgb;
   logic [11:0] reg_d1_reg, reg_d2_reg;
   logic [19:0] add_r;
   logic [5:0] plt_code;
   logic[19:0] temp;
   //assign temp = {9'b000000000, y[10:9], 9'b000000000};
   //assign addr_r = {10'b0000000000, y, x};
   assign addr_r = {y[9:0],x[9:0]};
   
   bkg_ram_unit #(.ADDR_WIDTH(20), .DATA_WIDTH(8)) bkg_ram (
      .clk(clk), .we(1'b0), .addr_w(20'h00000000), .din(20'h00000000),
      .addr_r(addr_r), .dout(plt_code));
   
      always_comb
      case (plt_code)
        6'b000000:	rgb = 12'h000;
        6'b000001:	rgb = 12'hfff;
        6'b000010:	rgb = 12'h630;
        6'b000011:	rgb = 12'h911;
        6'b000100:	rgb = 12'h860;
        6'b000101:	rgb = 12'hffd;
        6'b000110:	rgb = 12'hb72;
        6'b000111:	rgb = 12'h070;
        6'b001000:	rgb = 12'hd00;
        6'b001001:	rgb = 12'h760;
        6'b001010:	rgb = 12'hfc0;
        6'b001011:	rgb = 12'h871;
        6'b001100:	rgb = 12'h971;
        6'b001101:	rgb = 12'h671;
        6'b001110:	rgb = 12'ha72;
        6'b001111:	rgb = 12'ha71;
        6'b010000:	rgb = 12'h771;
        6'b010001:	rgb = 12'h371;
        6'b010010:	rgb = 12'ha62;
        6'b010011:	rgb = 12'h571;
        6'b010100:	rgb = 12'h471;
        6'b010101:	rgb = 12'h271;
        6'b010110:	rgb = 12'h270;
        6'b010111:	rgb = 12'h170;
        6'b011000:	rgb = 12'h99a;
        6'b011001:	rgb = 12'h171;
        6'b011010:	rgb = 12'h370;
        6'b011011:	rgb = 12'h861;
        6'b011100:	rgb = 12'h566;
        6'b011101:	rgb = 12'hbcc;
        6'b011110:	rgb = 12'h071;
        6'b011111:	rgb = 12'h722;
        6'b100000:	rgb = 12'h944;
        6'b100001:	rgb = 12'hff0;
        6'b100010:	rgb = 12'hbbc;
        6'b100011:	rgb = 12'h556;
        6'b100100:	rgb = 12'h222;
        6'b100101:	rgb = 12'h333;
        6'b100110:	rgb = 12'ha61;
        6'b100111:	rgb = 12'h2a0;
        default: rgb = 12'hfff;   // white
      endcase   
   
//   // body
//   assign up = x[6:3];
//   assign down = ~x[6:3];    // "not" reverse the binary sequence 
   
//   always_comb begin
//      // 16 shades of gray
//      if (y < 128) begin
//         r = x[8:5];
//         g = x[8:5];
//         b = x[8:5];
//      end   
//      // 8 prime colcor with 50% intensity
//      else if (y < 256) begin
//         r = {x[8], x[8], 2'b00};
//         g = {x[7], x[7], 2'b00};
//         b = {x[6], x[6], 2'b00};
//      end
//      else begin   
//      // a continuous color spectrum 
//         case (x[9:7]) 
//            3'b000: begin
//               r = 4'b1111;
//               g = up;
//               b = 4'b0000;
//            end   
//            3'b001: begin
//               r = down;
//               g = 4'b1111;
//               b = 4'b0000;
//            end   
//            3'b010: begin
//               r = 4'b0000;
//               g = 4'b1111;
//               b = up;
//            end   
//            3'b011: begin
//               r = 4'b0000;
//               g = down;
//               b = 4'b1111;
//            end   
//            3'b100: begin
//               r = up;
//               g = 4'b0000;
//               b = 4'b1111;
//            end   
//            3'b101: begin
//               r = 4'b1111;
//               g = 4'b0000;
//               b = down;
//            end   
//            default: begin
//               r = 4'b1111;
//               g = 4'b1111;
//               b = 4'b1111;
//            end  
//         endcase
//      end // else
//   end // always   
   // output with 2-stage delay line
   always_ff @(posedge clk) begin
      reg_d1_reg <= rgb;
      reg_d2_reg <= reg_d1_reg;
   end
   assign bar_rgb = reg_d2_reg;
endmodule