var ir_out2 = [];
var p_m = 0; //previous mode, required to generate 'OFF' mode bits

//LSB8 to MSB converter
function lsb8_to_msb()
{
    var reverse_num = 0;
    var val = 0;
    for (var j=0; j<11; j++)
    {
      for (var i=0; i<8; i++)
      {
        val = (ir_out2[j] & (1 << i));
        if(val)
        reverse_num |= (1 << ((8 - 1) - i));
      }
      ir_out2[j] = reverse_num;
      val = 0;
      reverse_num = 0;
    }  
}

function setswing(hsw, vsw)
{
    if (hsw)
    {
        ir_out2[5]=0x7F;
        ir_out2[6]=0x80;
    }
    else
    {
        ir_out2[5]=0xFF;
        ir_out2[6]=0x00;
    }
    if (vsw)
    {
        ir_out2[7]-=0x10;
        ir_out2[8]+=0x10;
    }
}

function setfan(fan)
{
    ir_out2[7] = 0x0F;
    ir_out2[8] = 0x00;
    switch (fan) {

        case 1: ;
            ir_out2[7] |= 0xF0;
            ir_out2[8] |= 0x0;
            break;

        case 2: ;
            ir_out2[7] |= 0x70;
            ir_out2[8] |= 0x80;
            break;

        case 3: ;
            ir_out2[7] |= 0x90;
            ir_out2[8] |= 0x60;
            break;

        case 4: ;
            ir_out2[7] |= 0xB0;
            ir_out2[8] |= 0x40;
            break;
            
    }

}

function settemp(temp)
{
    ir_out2[9] &= 0x0F;
    ir_out2[10] &= 0x0F;
    ir_out2[9] |= 0xE0;
    ir_out2[10] |= 0x10;
    
    for(var i=18; i<=30; i++)
    {   
      if (i==temp){break;}
      ir_out2[9]-=0x10;
      ir_out2[10]+=0x10;
    }
}
  
function setmode(mode)
{
    ir_out2[9] &= 0xF0;
    ir_out2[10] &= 0xF0;

    switch(mode) {

        case 1: ;               //fan
            ir_out2[9]=0x27;
            ir_out2[10]=0xD8;
            p_m=1;
            break;
        case 2: ;               //cool
            ir_out2[9]|=0x06;
            ir_out2[10]|=0x09;
            p_m=2;
            break;
        case 3: ;               //heat
            ir_out2[9]|=0x03;
            ir_out2[10]|=0x0C;
            p_m=3;
            break;
        case 4: ;               //dry
            ir_out2[9]|=0x05;
            ir_out2[10]|=0x0A;
            p_m=4;
            break;
        case 5: ;               //Off. Depends on the previous mode
            switch(p_m) {
                case 1: ;
                    ir_out2[9]|=0x0F;
                    ir_out2[10]|=0x00;
                    break;
                case 2: ;
                    ir_out2[9]|=0x0E;
                    ir_out2[10]|=0x01;
                    break;
                case 3: ;
                    ir_out2[9]|=0x0B;
                    ir_out2[11]|=0x04;
                    break;
                case 4: ;
                    ir_out2[9]|=0x0D;
                    ir_out2[11]|=0x02;
                    break;

            }
    }

}

function ir_generator(irInfo)
{   
    //Submit data in the following format to decode
    //Temperature[18-30],FanMode[1-4],HSW[0/1],VSW[0/1],Mode[1-5]
    //Mode: Fan=1, Cool=2, Heat=3, Humidity=4, Off=5
    //Fan mode: Auto=1, High=2, Med=3, Low=4

    ir_out2[0]=0x52;
    ir_out2[1]=0xAE;
    ir_out2[2]=0xC3;
    ir_out2[3]=0x26;
    ir_out2[4]=0xD9;
    
    settemp(irInfo.temp);
    setfan(irInfo.fan);
    setswing(irInfo.hsw, irInfo.vsw);
    setmode(irInfo.mode);
    lsb8_to_msb();   
    
    console.log(ir_out2);

    console.log('\nFormed Data');
    var stringer = "[";
    ir_out2.forEach(function(element){
        stringer += ('0x' +element.toString(16) + ", ");
    });
    stringer += "]";
    console.log(stringer);

}

var xyz = {};
xyz.temp = 23;
xyz.fan = 2;
xyz.hsw = 0;
xyz.vsw = 1;
xyz.mode = 2;

ir_generator(xyz);