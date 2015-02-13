cd ../../../cc
svn up
./build now
scp -r liyuanjian@db-testing-dpwk08.db01:/home/liyuanjian/project/public/gm/galileo/output ~/public/gm/galileo/
./build
if [ $? -eq 0 ]
then 
 cp pm ../test/integrate_test/word_cnt_test/configs/

 cp pn ../test/integrate_test/word_cnt_test/configs/
 else
  echo "WRONG TO COMPILE PN&PM"
fi
