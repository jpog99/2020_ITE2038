select p.name
from pokemon p,

(select type 
from pokemon 
group by type order by count(type) desc limit 4) as q

where p.type = q.type order by name
