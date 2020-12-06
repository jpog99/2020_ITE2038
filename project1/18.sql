SELECT avg(level)
from catchedpokemon 
where owner_id in (select leader_id from gym)
