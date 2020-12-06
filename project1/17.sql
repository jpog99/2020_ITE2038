select count(*) as typeCount
from (
  SELECT count(type)
  FROM CatchedPokemon INNER JOIN Pokemon AS p ON pid=p.id 
  WHERE owner_id IN (SELECT id FROM Trainer WHERE hometown='Sangnok City')
  GROUP BY type) as p
