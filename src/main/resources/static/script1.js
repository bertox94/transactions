


//imposta width input in tabella appropriato
//inserimento di nuovo elemento ordinato
//elimaxa selezionati
//aggiorna dipendente quando metto nuova avlore input, dopo tasto conferma


//n2 100 dipendenti

var table=document.getElementsByTagName("table");
        var data;

        function exportData(){

            $.get("/get",function(data){
            data=JSON.parse(data);

                var outExport="";

                if($("#id8").is(':checked')){
                outExport="<ANannullamento>1</ANannullamento>";
                }else{
                outExport="<ANannullamento>0</ANannullamento>";
                }


                var j=0;
                for(var i=0;i<$("#id3 tr").length;i++){

                // if checked

                if($("input:checkbox").eq(i).is(':checked')){

                j++;
                if(data[i][2]!="")
                outExport=outExport+"<CFlavoratore"+j+">"+data[i][2]+"</CFlavoratore"+j+">";
                else
                outExport=outExport+"<CFlavoratore"+j+" />";

                if(data[i][3]!="")
                outExport=outExport+"<CCcodcomunicazione"+j+">"+data[i][3]+"</CCcodcomunicazione"+j+">";
                else
                outExport=outExport+"<CCcodcomunicazione"+j+" />";

                outExport=outExport+"<DTdatainizio"+j+" />";

                outExport=outExport+"<DTdatafine"+j+" />";

                }

                }

            $.post("/export",{data:outExport},function(retData){
                        window.location.href="/download";

                        });
            });
        }

        function deleteRow(){

        var iter=$("input:checked");

        for(var k=0;k<iter.length;k++){
        $("input:checked").eq(0).parent().parent().remove();
        }

        i=0;
        data=[];
        var rows=$("#id3 tr");

        while(i<rows.length){
        data[i]=[];
        data[i][0]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(0).attr("placeholder");
        data[i][1]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(1).attr("placeholder");
        data[i][2]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(2).attr("placeholder");
        data[i][3]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(3).attr("placeholder");
        data[i][4]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(4).attr("placeholder");
        data[i][5]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(5).attr("placeholder");
        i++;
        }

        $.post("/update",
        {
        data:JSON.stringify(data)
        });

        }


        function applyEdit(){


        var rows=$("#id3 tr");
        var iter=$(".CF");
        var cf=$("#CFinput");
        var i=0;
        var k=0;

        while(i<rows.length){

        if(iter[i].value==cf[0].value){


        $("#id4").remove();
        $("#id3").after('<tr id = "id4"><td><input class = "newResource" type="text" placeholder="Nome"></input></td><td><input class = "newResource" type="text" placeholder="Cognome"></input></td><td><input id="CFinput" class = "newResource" type="text" placeholder="Codice fiscale"></input></td><td><input class = "newResource" type="text" placeholder="Codice comunicazione"></input></td><td><input class = "newResource" type="text" placeholder="Indirizzo"></input></td><td><input class = "newResource" type="text" placeholder="Telefono"></input></td></tr>')


        return;
        }
        i++;
        }


        var i=0;
        var k=0;

        while(i<rows.length){
        while(k<rows.length){
        if(i!=k&&iter[i].value==iter[k].value){

        iter[k].value=iter[k].placeholder;
        iter[i].value=iter[i].placeholder;

        return;
        }
        k++;
        }
        i++;
        }

        i=0;

        var iter=$("input").not(":checkbox");
        for(var k=0;k<iter.length;k++){
        if(iter[k].value!="")
        iter[k].placeholder=iter[k].value;
        if(iter[k].class =="newResource"){
        iter[k].value="";
        }
        }


        $("#id4").remove();
        $("#id3").after('<tr id = "id4"><td><input class = "newResource" type="text" placeholder="Nome"></input></td><td><input class = "newResource" type="text" placeholder="Cognome"></input></td><td><input id="CFinput" class = "newResource" type="text" placeholder="Codice fiscale"></input></td><td><input class = "newResource" type="text" placeholder="Codice comunicazione"></input></td><td><input class = "newResource" type="text" placeholder="Indirizzo"></input></td><td><input class = "newResource" type="text" placeholder="Telefono"></input></td></tr>')


        var rows=$("#id3 tr");
        i=0;
        data=[];

        while(i<rows.length){
        data[i]=[];
        data[i][0]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(0).attr("placeholder");
        data[i][1]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(1).attr("placeholder");
        data[i][2]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(2).attr("placeholder");
        data[i][3]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(3).attr("placeholder");
        data[i][4]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(4).attr("placeholder");
        data[i][5]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(5).attr("placeholder");
        i++;
        }

        $.post("/update",
        {
        data:JSON.stringify(data)
        });


        }

        function insertInOrder(){


        var rows=$("#id3 tr");
        var iter=$(".CF");
        var cf=$("#CFinput");
        var i=0;
        var k=0;

        while(i<rows.length){
        while(k<rows.length){
        if(i!=k&&iter[i].value==iter[k].value){

        iter[k].value=iter[k].placeholder;
        iter[i].value=iter[i].placeholder;

        return;
        }
        k++;
        }
        i++;
        }

        if($("#id4 td input").eq(2).val()==""||$("#id4 td input").eq(1).val()==""||$("#id4 td input").eq(0).val()=="")
        return;

        var rows=$("#id3 tr");
        var iter=$(".CF");
        var cf=$("#CFinput");
        var i=0;

        while(i<rows.length){
        if(iter[i].value==cf[0].value)
        return;
        i++;
        }


// controllA NOME COGNOME CODICE FISCALE PRESENTI ERFORZA
        var arr=[]

        var rows=$("#id3 tr");
        var i=0;

        while(i<rows.length){
        rows[i]=[];
        rows[i][0]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(0).attr("placeholder");
        rows[i][1]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(1).attr("placeholder");
        rows[i][2]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(2).attr("placeholder");
        rows[i][3]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(3).attr("placeholder");
        rows[i][4]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(4).attr("placeholder");
        rows[i][5]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(5).attr("placeholder");
        i++;
        }


        var newDip=[];

        newDip[0]=$("#id4 td .newResource").eq(0).val();
        newDip[1]=$("#id4 td .newResource").eq(1).val();
        newDip[2]=$("#id4 td .newResource").eq(2).val();
        newDip[3]=$("#id4 td .newResource").eq(3).val();
        newDip[4]=$("#id4 td .newResource").eq(4).val();
        newDip[5]=$("#id4 td .newResource").eq(5).val();

//ordinamento

        for(i=0;i<rows.length;i++){
            if($("#id4 td .newResource").eq(1).val()<(rows[i][1])){
                break;
            }
        }

        if(rows.length==0){
            $("#id3").append('<tr>   <td>	<input class="Nome" type="text"	value="'+$("#id4 td .newResource").eq(0).val()+'" placeholder="'+$("#id4 td .newResource").eq(0).val()+'"></input></td>        <td>	<input class="Nome" type="text"	value="'+$("#id4 td .newResource").eq(1).val()+'" placeholder="'+$("#id4 td .newResource").eq(1).val()+'"></input></td> <td> <input type="checkbox"></input>	<input class="CF" type="text"	value="'+$("#id4 td .newResource").eq(2).val()+'" placeholder="'+$("#id4 td .newResource").eq(2).val()+'"></input></td> <td>	<input class="nome" type="text"	value="'+$("#id4 td .newResource").eq(3).val()+'" placeholder="'+$("#id4 td .newResource").eq(3).val()+'"></input></td> <td>	<input class="nome" type="text" value="'+$("#id4 td .newResource").eq(4).val()+'" placeholder="'+$("#id4 td .newResource").eq(4).val()+'"></input></td> <td>	<input class="nome" type="text"	value="'+$("#id4 td .newResource").eq(5).val()+'" placeholder="'+$("#id4 td .newResource").eq(5).val()+'"></input></td>                   </tr>');
        } else if (i == rows.length) {
            $("#id3 tr").eq(i-1).after('<tr>   <td>	<input class="Nome" type="text"	value="'+$("#id4 td .newResource").eq(0).val()+'" placeholder="'+$("#id4 td .newResource").eq(0).val()+'"></input></td>        <td>	<input class="Nome" type="text"	value="'+$("#id4 td .newResource").eq(1).val()+'" placeholder="'+$("#id4 td .newResource").eq(1).val()+'"></input></td> <td> <input type="checkbox"></input>	<input class="CF" type="text"	value="'+$("#id4 td .newResource").eq(2).val()+'" placeholder="'+$("#id4 td .newResource").eq(2).val()+'"></input></td> <td>	<input class="nome" type="text"	value="'+$("#id4 td .newResource").eq(3).val()+'" placeholder="'+$("#id4 td .newResource").eq(3).val()+'"></input></td> <td>	<input class="nome" type="text" value="'+$("#id4 td .newResource").eq(4).val()+'" placeholder="'+$("#id4 td .newResource").eq(4).val()+'"></input></td> <td>	<input class="nome" type="text"	value="'+$("#id4 td .newResource").eq(5).val()+'" placeholder="'+$("#id4 td .newResource").eq(5).val()+'"></input></td>                   </tr>');
        } else {
            $("#id3 tr").eq(i).before('<tr>   <td>	<input class="Nome" type="text"	value="'+$("#id4 td .newResource").eq(0).val()+'" placeholder="'+$("#id4 td .newResource").eq(0).val()+'"></input></td>        <td>	<input class="Nome" type="text"	value="'+$("#id4 td .newResource").eq(1).val()+'" placeholder="'+$("#id4 td .newResource").eq(1).val()+'"></input></td> <td> <input type="checkbox"></input>	<input class="CF" type="text"	value="'+$("#id4 td .newResource").eq(2).val()+'" placeholder="'+$("#id4 td .newResource").eq(2).val()+'"></input></td> <td>	<input class="nome" type="text"	value="'+$("#id4 td .newResource").eq(3).val()+'" placeholder="'+$("#id4 td .newResource").eq(3).val()+'"></input></td> <td>	<input class="nome" type="text" value="'+$("#id4 td .newResource").eq(4).val()+'" placeholder="'+$("#id4 td .newResource").eq(4).val()+'"></input></td> <td>	<input class="nome" type="text"	value="'+$("#id4 td .newResource").eq(5).val()+'" placeholder="'+$("#id4 td .newResource").eq(5).val()+'"></input></td>                   </tr>');
        }


        $("#id4").remove();
        $("#id3").after('<tr id = "id4"><td><input class = "newResource" type="text" placeholder="Nome"></input></td><td><input class = "newResource" type="text" placeholder="Cognome"></input></td><td><input id ="CFinput" class = "newResource" type="text" placeholder="Codice fiscale"></input></td><td><input class = "newResource" type="text" placeholder="Codice comunicazione"></input></td><td><input class = "newResource" type="text" placeholder="Indirizzo"></input></td><td><input class = "newResource" type="text" placeholder="Telefono"></input></td></tr>')


        var input=document.querySelectorAll('input');
        for(i=0;i<input.length;i++){
        if(input[i].type!="button"&&input[i].type!="checkbox")
        input[i].setAttribute('size',max(input[i].getAttribute('placeholder').length,20));
        }


        // $("#id3").find("input").attr('size',$(this).attr('placeholder').length);
        // $("#id3 tbody tr td
        // input").attr('size',$(this).attr('placeholder').length);

        applyEdit();


        var rows=$("#id3 tr");

        i=0;
        data=[];

        while(i<rows.length){

        if (!data[i]) data[i] = [];

        data[i][0]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(0).attr("placeholder");
        data[i][1]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(1).attr("placeholder");
        data[i][2]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(2).attr("placeholder");
        data[i][3]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(3).attr("placeholder");
        data[i][4]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(4).attr("placeholder");
        data[i][5]=$("#id3 tr").eq(i).find("input").not(":checkbox").eq(5).attr("placeholder");
        i++;
        }


        $.post("/update",
        {
        data:JSON.stringify(data)
        });
        }

        function max(n1,n2){

        /*
		 * var n1 = parseInt(n1); var n2 = parseInt(n2);
		 */

        if(n1>n2)
        return n1;
        return n2;

        }


        $(document).ready(function(){


        $.get("/get",function(data){

        data=JSON.parse(data);
        var i=0;

        while(data[i]!=undefined){
        // style="margin-right:10px;"
        $("#id3").append('<tr><td>	<input class="Nome" type="text" value ="'+data[i][0]+'"	 placeholder="'+data[i][0]+'"></input></td> <td>	<input class="nome" type="text"	value ="'+data[i][1]+'" placeholder="'+data[i][1]+'"></input></td> <td> <input type="checkbox"></input>	<input class="CF" type="text"	value ="'+data[i][2]+'" placeholder="'+data[i][2]+'"></input></td> <td>	<input class="nome" value ="'+data[i][3]+'" type="text" placeholder="'+data[i][3]+'"></input></td> <td>	<input class="nome" value ="'+data[i][4]+'" type="text"	 placeholder="'+data[i][4]+'"></input></td>     <td>	<input class="nome" type="text"	value ="'+data[i][5]+'" placeholder="'+data[i][5]+'"></input></td>                </tr>');
        i++;

        }
        var input=document.querySelectorAll('input');
        for(i=0;i<input.length;i++){
        if(input[i].type!="button"&&input[i].type!="checkbox")
        input[i].setAttribute('size',max(input[i].getAttribute('placeholder').length,20));
        }

        if(data==null){

        $("*").not("#id2").not("html").not("body").remove();
        $("#id2").after("</br></br><font color='red'>Error: web storage undefined</font>");

        }
        
        alert("Data Loaded: "+data);

        });

        });
        