
google.charts.load('current', {'packages':['corechart', 'line']});
google.charts.setOnLoadCallback(drawChartDist);
google.charts.setOnLoadCallback(drawChartTemp);

function get_plot_data(topic)
{
    let return_value = "Could not use plot-API";
    $.ajax({
        url: "http://127.0.0.1:5000/" + topic,
        type: 'GET',
        crossDomain: true,
        // data: {'file': path},
        async: false,
        success: function(data) {
            return_value = data;
        }
    });
    return return_value;
}


function drawChartDist() {

    var data = new google.visualization.DataTable();

    let plot_data = get_plot_data('distance');

    let dists = plot_data['data'];
    let counter = plot_data['counter'];

    // console.log(dists);


    data.addColumn('date', 'X');

    // let i=0;
    for (let i = 0; i < counter; i++) {
        data.addColumn('number', 'Distance' + i.toString());
    }

    for (let i in dists)
    {
        // if (i == 10)
        //     break;
        console.log(i);
        console.log(dists[i]);
        let d =  new Date(dists[i][0]*1000);
        data.addRow([d].concat(dists[i].splice(1)));
    }

    var options = {
        hAxis: {
            title: 'Time'
        },
        vAxis: {
            title: 'Distance'
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
    chart.draw(data, options);
}

function bla()
{
    let plot_data = get_plot_data('wrongdoers');
    // console.log(['Nikolas']);

    let user_names = ['Nikolas', "George", "Michael"];

    let user_data = plot_data['wrongdoers'];

    for (let u_id in user_names)
    {
        let user = user_names[u_id];
        // console.log(user);
        // console.log(user_data[user]);
        document.getElementById(user).innerHTML = user_data[user].toString();
    }
}


function drawChartTemp() {

    bla();

    var data = new google.visualization.DataTable();
    let plot_data = get_plot_data('temperature');

    let temps = plot_data['temps']
    data.addColumn('date', 'X');
    data.addColumn('number', 'Temperature');

    for (let i in temps)
    {
        let d =  new Date(temps[i][0]*1000);
        data.addRow([d, temps[i][1]]);
        // if (i == 10)
        //     break
    }

    var options = {
        hAxis: {
            title: 'Time'
        },
        vAxis: {
            title: 'Temperature'
        }
    };

    var chart = new google.visualization.LineChart(document.getElementById('temp_chart_div'));
    chart.draw(data, options);
}

