from dataclasses import asdict
from typing import List

from opensearchpy import exceptions
from wazuh.core.exception import WazuhError, WazuhResourceNotFound

from .base import BaseIndex
from .constants import (
    BODY_KEY,
    INDEX_KEY,
    QUERY_KEY,
    SOURCE_KEY,
    TERMS_KEY,
)
from .models import Agent


class AgentsIndex(BaseIndex):
    """Set of methods to interact with `agents` index."""

    INDEX = 'agents'
    SECONDARY_INDEXES = []

    async def create(self, id: str, key: str, name: str) -> Agent:
        """Create a new agent.

        Parameters
        ----------
        id : str
            Identifier of the new agent.
        key : str
            Key of the new agent.
        name : str
            Name of the new agent.

        Returns
        -------
        Agent
            The created agent instance.

        Raises
        ------
        WazuhError (1708)
            When already exists an agent with the provided id.
        """
        agent = Agent(id=id, key=key, name=name)
        try:
            await self._client.index(
                index=self.INDEX,
                id=agent.id,
                body=asdict(agent),
                op_type='create',
                refresh='wait_for'
            )
        except exceptions.ConflictError:
            raise WazuhError(1708, extra_message=id)
        else:
            return agent

    async def delete(self, ids: List[str]) -> list:
        """Delete multiple agents that match with the given parameters.

        Parameters
        ----------
        ids : List[str]
            Agent ids to delete.

        Returns
        -------
        list
            Ids of the deleted agents.
        """
        indexes = ','.join([self.INDEX, *self.SECONDARY_INDEXES])
        body = {QUERY_KEY: {TERMS_KEY: {'_id': ids}}}
        parameters = {INDEX_KEY: indexes, BODY_KEY: body, 'conflicts': 'proceed'}

        await self._client.delete_by_query(**parameters, refresh='true')

        return ids

    async def search(self, query: dict) -> dict:
        """Perform a search operation with the given query.

        Parameters
        ----------
        query : dict
            DSL query.

        Returns
        -------
        dict
            The search result.
        """
        parameters = {INDEX_KEY: self.INDEX, BODY_KEY: query}
        return await self._client.search(**parameters)

    async def get(self, id: str) -> Agent:
        """Get an agent.

        Parameters
        ----------
        id : str
            Agent ID.

        Returns
        -------
        Agent
            Agent instance.
        """
        try:
            data = self._client.get(index=self.INDEX, id=self.id)
        except exceptions.NotFoundError:
            raise WazuhResourceNotFound(1701)
        finally:
            return Agent(uuid=id, **data[SOURCE_KEY])

