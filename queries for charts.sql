-- QUERY FOR THE TABLE
select *
from preview p
where planneddate is not null;

-- QUERIES FOR THE FIRST CHART
select executiondate
from preview p
group by executiondate
order by executiondate;

select sum(balance) as balance
from preview p
group by executiondate
order by executiondate;


select m,
       (select avg(balance)
        from (select sum(balance) as balance
              from preview p
              group by executiondate
              order by executiondate
             ) as subq_b
       ) - m * (select (count(*) + 1) / 2.0
                from (
                         select distinct executiondate
                         from preview p
                     ) as subq2
       ) as q
from (
         select _num / _denom as m
         from (
                  select sum(balance_a * x_a) as _num
                  from (
                           select balance - (select avg(balance)
                                             from (
                                                      select sum(balance) as balance
                                                      from preview p
                                                      group by executiondate
                                                      order by executiondate
                                                  ) as subq1) as balance_a,
                                  x_a
                           from (
                                    select sum(balance) as balance, executiondate
                                    from preview p
                                    group by executiondate
                                    order by executiondate
                                ) as subq2
                                    full outer join (
                               select ROW_NUMBER() OVER (ORDER BY executiondate) - (select (count(*) + 1) / 2.0
                                                                                    from (
                                                                                             select distinct executiondate
                                                                                             from preview p
                                                                                         ) as subq2
                               ) as x_a,
                                      executiondate
                               from (
                                        select distinct executiondate
                                        from preview p
                                    ) as subq1
                           ) as subq3 on subq2.executiondate = subq3.executiondate
                           where subq2.executiondate is not null
                              or subq3.executiondate is not null
                       ) as subq4
              ) as sub6,
              (
                  select sum(x_a ^ 2) as _denom
                  from (
                           select ROW_NUMBER() OVER (ORDER BY executiondate) - (select (count(*) + 1) / 2.0
                                                                                from (
                                                                                         select distinct executiondate
                                                                                         from preview p
                                                                                     ) as subq2) as x_a
                           from (
                                    select distinct executiondate
                                    from preview p
                                ) as subq1
                       ) as subq5
              ) as sub7
     ) as sub14;


-- QUERIES FOR THE SECOND CHART
select executiondate
from preview p
where planneddate is not null;

select amount
from preview p
where planneddate is not null;

select balance
from preview p
where planneddate is not null;

