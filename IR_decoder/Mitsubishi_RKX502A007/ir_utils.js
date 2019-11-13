// IR utility methods


class IRUtils {

    static binaryReverse(data)
    {
        var revNum = 0,
            i =0,
            num = data.number,
            pow = data.pow;


        for( i = 0; i < pow; i++)
        {
            revNum = revNum | (num>>i & 1) ;
            revNum = revNum<<1;
        }	

        revNum = revNum >> 1; 

        return revNum;
    }

    static binaryCompliment(data)
    {
        var revNum = 0,
            i =0,
            num = data.number,
            pow = data.pow;

            revNum = num ^ (Math.pow(2,pow) -1);

        return revNum;
    }		
    
}
module.exports = IRUtils;
